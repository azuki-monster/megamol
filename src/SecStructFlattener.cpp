/*
 *	SecStructFlattener.cpp
 *
 *	Copyright (C) 2016 by Universitaet Stuttgart (VISUS).
 *	All rights reserved
 */

#include "stdafx.h"
#include "SecStructFlattener.h"
#include "PlaneDataCall.h"

#include "mmcore/param/BoolParam.h"
#include "mmcore/param/ButtonParam.h"
#include "mmcore/param/EnumParam.h"
#include "mmcore/param/Vector3fParam.h"

#include "vislib/math/Matrix.h"
#include "vislib/math/ShallowVector.h"
#include "vislib/sys/Log.h"

using namespace megamol;
using namespace megamol::core;
using namespace megamol::protein_cuda;
using namespace megamol::protein_calls;

/*
 *	SecStructFlattener::SecStructFlattener
 */
SecStructFlattener::SecStructFlattener(void) :
	Module(),
	getDataSlot("getData", "Calls molecular data"),
	dataOutSlot("dataOut", "Provides the flattened molecular data"),
	planeOutSlot("planeOut", "Provides the necessary plane data for 2D renderings"),
	playParam("animation::play", "Should the animation be played?"),
	playButtonParam("animation::playButton", "Button to toggle animation"),
	flatPlaneMode("plane::flatPlaneMode", "The plane the protein gets flattened to"),
	arbPlaneCenterParam("plane::planeOrigin", "The plane origin for the arbitrary plane mode"),
	arbPlaneNormalParam("plane::planeNormal", "The plane normal for the arbitrary plane mode"),
	oxygenOffsetParam("plane::preserveDirections", "Should the directions between c alpha and oxygen atoms be preserved?"){

	// caller slot
	this->getDataSlot.SetCompatibleCall<MolecularDataCallDescription>();
	this->MakeSlotAvailable(&this->getDataSlot);

	// callee slot
	this->dataOutSlot.SetCallback(MolecularDataCall::ClassName(), MolecularDataCall::FunctionName(0), &SecStructFlattener::getData);
	this->dataOutSlot.SetCallback(MolecularDataCall::ClassName(), MolecularDataCall::FunctionName(1), &SecStructFlattener::getExtent);
	this->MakeSlotAvailable(&this->dataOutSlot);

	this->planeOutSlot.SetCallback(PlaneDataCall::ClassName(), PlaneDataCall::FunctionName(0), &SecStructFlattener::getPlaneData);
	this->planeOutSlot.SetCallback(PlaneDataCall::ClassName(), PlaneDataCall::FunctionName(1), &SecStructFlattener::getPlaneExtent);
	this->MakeSlotAvailable(&this->planeOutSlot);

	this->playParam.SetParameter(new param::BoolParam(false));
	this->MakeSlotAvailable(&this->playParam);

	this->playButtonParam << new param::ButtonParam('p');
	this->playButtonParam.SetUpdateCallback(this, &SecStructFlattener::onPlayToggleButton);
	this->MakeSlotAvailable(&this->playButtonParam);

	param::EnumParam * fpParam = new param::EnumParam(int(FlatPlane::XY_PLANE));
	FlatPlane fp;
	for (int i = 0; i < getFlatPlaneModeNumber(); i++) {
		fp = getFlatPlaneByIndex(i);
		fpParam->SetTypePair(fp, getFlatPlaneName(fp).c_str());
	}
	this->flatPlaneMode << fpParam;
	this->MakeSlotAvailable(&this->flatPlaneMode);

	const vislib::math::Vector<float, 3> orig(0.0f, 0.0f, 0.0f);
	this->arbPlaneCenterParam.SetParameter(new param::Vector3fParam(orig));
	this->MakeSlotAvailable(&this->arbPlaneCenterParam);

	const vislib::math::Vector<float, 3> orig2(0.0f, 0.0f, 1.0f);
	this->arbPlaneNormalParam.SetParameter(new param::Vector3fParam(orig2));
	this->MakeSlotAvailable(&this->arbPlaneNormalParam);

	this->oxygenOffsetParam.SetParameter(new param::BoolParam(true));
	this->MakeSlotAvailable(&this->oxygenOffsetParam);

	this->atomPositions = NULL;
	this->atomPositionsSize = 0;

	this->lastHash = 0;
	this->hashOffset = 0;
	this->myHash = 0;
	this->planeHash = 0;
	this->firstFrame = true;

	this->lastPlaneMode = XY_PLANE;

	this->flatPlaneMode.ForceSetDirty();
	this->mainDirections.resize(3);
}

/*
 *	SecStructFlattener::~SecStructFlattener
 */
SecStructFlattener::~SecStructFlattener(void) {
	this->Release();
}

/*
 *	SecStructFlattener::create
 */
bool SecStructFlattener::create(void) {
	return true;
}

/*
 *	SecStructFlattener::release
 */
void SecStructFlattener::release(void) {
	if (this->atomPositions != NULL) {
		delete[] this->atomPositions;
		this->atomPositions = NULL;
		this->atomPositionsSize = 0;
	}
}

/*
 *	SecStructFlattener::computeMainDirectionPCA
 */
void SecStructFlattener::computeMainDirectionPCA(void) {
	vislib::math::Matrix<float, 3, vislib::math::ROW_MAJOR> covMat;
	covMat.SetNull();

	// compute the midpoint of the data set
	vislib::math::Vector<float, 3> midpoint(0.0f, 0.0f, 0.0f);
	for (unsigned int k = 0; k < cAlphaIndices.size(); k++) {
		vislib::math::ShallowVector<const float, 3> p1(&this->atomPositions[this->cAlphaIndices[k] * 3]);
		midpoint += p1;
	}
	midpoint /= static_cast<float>(cAlphaIndices.size());

	// compute covariance matrix
	for (unsigned int x = 0; x < 3; x++) {
		for (unsigned int y = 0; y < 3; y++) {
			for (unsigned int k = 0; k < cAlphaIndices.size(); k++) {
				vislib::math::ShallowVector<const float, 3> p1(&this->atomPositions[this->cAlphaIndices[k] * 3]);
				covMat(x, y) += (p1[x] - midpoint[x]) * (p1[y] - midpoint[y]);
			}
			covMat(x, y) /= static_cast<float>(cAlphaIndices.size() - 1);
		}
	}
	//covMat.Dump(std::cout);

	float eigenVals[3];
	vislib::math::Vector<float, 3> eigenVectors[3];
	covMat.FindEigenvalues(eigenVals, eigenVectors, 3);
	std::vector<unsigned int> indexVec = { 0, 1, 2 };

	std::sort(indexVec.begin(), indexVec.end(), [&eigenVals](const unsigned int& a, const unsigned int& b) {
		return eigenVals[a] > eigenVals[b];
	});

	for (int i = 0; i < 3; i++) {
		mainDirections[i] = eigenVectors[indexVec[i]];
		mainDirections[i].Normalise();
	}
}

/*
 *	SecStructFlattener::flatten
 */
void SecStructFlattener::flatten(void) {

	auto bbCenter = this->boundingBox.CalcCenter();
	vislib::math::Vector<float, 3> bbCenterVec(bbCenter.GetX(), bbCenter.GetY(), bbCenter.GetZ());
	vislib::math::Vector<float, 3> origin(0.0f, 0.0f, 0.0f);

	if (this->firstFrame) {
		this->arbPlaneCenterParam.Param<param::Vector3fParam>()->SetValue(origin);
		this->firstFrame = false;
	} else {
		bbCenterVec = this->arbPlaneCenterParam.Param<param::Vector3fParam>()->Value();
	}

	if (this->flatPlaneMode.IsDirty() || this->arbPlaneCenterParam.IsDirty() || this->arbPlaneNormalParam.IsDirty() || this->oxygenOffsetParam.IsDirty()) {
		this->flatPlaneMode.ResetDirty();
		this->arbPlaneCenterParam.ResetDirty();
		this->arbPlaneNormalParam.ResetDirty();
		this->oxygenOffsetParam.ResetDirty();
		this->hashOffset++;

		vislib::math::Vector<float, 3> n = this->arbPlaneNormalParam.Param<param::Vector3fParam>()->Value();
		n.Normalise();

		switch (this->flatPlaneMode.Param<param::EnumParam>()->Value()) {
			case XY_PLANE:
				for (unsigned int i = 0; i < this->atomPositionsSize / 3; i++) {
					this->atomPositions[i * 3 + 2] = bbCenterVec.GetZ();
				}
				break;
			case XZ_PLANE:
				for (unsigned int i = 0; i < this->atomPositionsSize / 3; i++) {
					this->atomPositions[i * 3 + 1] = bbCenterVec.GetY();
				}
				break;
			case YZ_PLANE:
				for (unsigned int i = 0; i < this->atomPositionsSize / 3; i++) {
					this->atomPositions[i * 3 + 0] = bbCenterVec.GetX();
				}
				break;
			case LEAST_COMMON:
				computeMainDirectionPCA();
				// project points onto plane
				for (unsigned int i = 0; i < this->atomPositionsSize / 3; i++) {
					vislib::math::ShallowVector<float, 3> atomPos(&this->atomPositions[i * 3]);
					vislib::math::Vector<float, 3> v(&this->atomPositions[i * 3]);
					v = v - bbCenterVec;
					float dist = v.Dot(mainDirections[2]);
					atomPos = atomPos - dist * mainDirections[2];
				}
				break;
			case ARBITRARY:
				// project points onto plane
				for (unsigned int i = 0; i < this->atomPositionsSize / 3; i++) {
					vislib::math::ShallowVector<float, 3> atomPos(&this->atomPositions[i * 3]);
					vislib::math::Vector<float, 3> v(&this->atomPositions[i * 3]);
					v = v - bbCenterVec;
					float dist = v.Dot(n);
					atomPos = atomPos - dist * n;
				}
				break;
			default:
				break;
		}

		if (this->oxygenOffsetParam.Param<param::BoolParam>()->Value()) {
			for (unsigned int i = 0; i < this->oxygenOffsets.size(); i++) {
				vislib::math::ShallowVector<float, 3> oPos(&this->atomPositions[oIndices[i] * 3]);
				vislib::math::ShallowVector<float, 3> cPos(&this->atomPositions[cAlphaIndices[i] * 3]);
				oPos = cPos + oxygenOffsets[i];
			}
		}

		planeHash++;
	}
}

/*
 *	SecStructFlattener::getFlatPlaneByIndex
 */
SecStructFlattener::FlatPlane SecStructFlattener::getFlatPlaneByIndex(unsigned int idx) {
	switch (idx) {
		case 0:		return FlatPlane::XY_PLANE;
		case 1:		return FlatPlane::XZ_PLANE;
		case 2:		return FlatPlane::YZ_PLANE;
		case 3:		return FlatPlane::LEAST_COMMON;
		case 4:		return FlatPlane::ARBITRARY;
		default:	return FlatPlane::XY_PLANE;
	}
}

/*
 *	SecStructFlattener::getFlatPlaneModeNumber
 */
int SecStructFlattener::getFlatPlaneModeNumber(void) {
	return 5;
}

/*
 *	SecStructFlattener::getFlatPlaneName
 */
std::string SecStructFlattener::getFlatPlaneName(SecStructFlattener::FlatPlane fp) {
	switch (fp) {
		case XY_PLANE		: return "XY Plane";
		case XZ_PLANE		: return "XZ Plane";
		case YZ_PLANE		: return "YZ Plane";
		case LEAST_COMMON	: return "Least Common";
		case ARBITRARY		: return "Arbitrary";
		default				: return "";
	}
}

/*
 *	SecStructFlattener::getData
 */
bool SecStructFlattener::getData(core::Call& call) {
	MolecularDataCall * outCall = dynamic_cast<MolecularDataCall*>(&call);
	if (outCall == NULL) return false;

	this->myHash = this->lastHash + this->hashOffset;

	outCall->SetDataHash(this->myHash);
	outCall->SetAtomPositions(this->atomPositions);

	outCall->AccessBoundingBoxes().SetObjectSpaceBBox(this->boundingBox);
	outCall->AccessBoundingBoxes().SetObjectSpaceClipBox(this->boundingBox);

	return true;
}

/*
 *	SecStructFlattener::getExtent
 */
bool SecStructFlattener::getExtent(core::Call& call) {
	
	MolecularDataCall * agdc = dynamic_cast<MolecularDataCall*>(&call);
	if (agdc == NULL) return false;

	MolecularDataCall *mdc = this->getDataSlot.CallAs<MolecularDataCall>();
	if (mdc == NULL) return false;
	mdc->SetCalltime(agdc->Calltime());
	if (!(*mdc)(1)) return false;
	if (!(*mdc)(0)) return false;

	agdc->operator=(*mdc); // deep copy

	std::vector<float> atomRadii(mdc->AtomCount(), 0.0f);

	if (lastHash != mdc->DataHash() || this->flatPlaneMode.IsDirty() || this->arbPlaneCenterParam.IsDirty() || this->arbPlaneNormalParam.IsDirty()) {
		lastHash = mdc->DataHash();
		this->boundingBox = mdc->AccessBoundingBoxes().ObjectSpaceBBox();

		// copy the atom positions to the array used here
		if (atomPositions != NULL) {
			delete[] this->atomPositions;
			this->atomPositions = NULL;
			this->atomPositionsSize = 0;
		}

		atomPositions = new float[mdc->AtomCount() * 3];
		atomPositionsSize = mdc->AtomCount() * 3;
		cAlphaIndices.clear();
		oIndices.clear();

		for (unsigned int i = 0; i < mdc->AtomCount(); i++) {
			atomPositions[i * 3 + 0] = mdc->AtomPositions()[i * 3 + 0];
			atomPositions[i * 3 + 1] = mdc->AtomPositions()[i * 3 + 1];
			atomPositions[i * 3 + 2] = mdc->AtomPositions()[i * 3 + 2];
			atomRadii[i] = mdc->AtomTypes()[mdc->AtomTypeIndices()[i]].Radius();

			// check the relevant atom types
			vislib::StringA elName = mdc->AtomTypes()[mdc->AtomTypeIndices()[i]].Name();
			elName.ToLowerCase();
			elName.TrimSpaces();
			if (elName.StartsWith("ca")) cAlphaIndices.push_back(i);
			if (elName.StartsWith("o") && elName.Length() == 1) oIndices.push_back(i); // cut out all o atoms besides the first per aminoacid
		}

		if (cAlphaIndices.size() != oIndices.size()) {
			vislib::sys::Log::DefaultLog.WriteMsg(vislib::sys::Log::LEVEL_ERROR,
				"Malformed molecule (different number of c alpha and primary oxygen atoms)\n");
		}

		this->oxygenOffsets.resize(cAlphaIndices.size());
		for (unsigned int i = 0; i < cAlphaIndices.size(); i++) {
			vislib::math::ShallowVector<float, 3> cPos(&atomPositions[cAlphaIndices[i] * 3]);
			vislib::math::ShallowVector<float, 3> oPos(&atomPositions[oIndices[i] * 3]);
			this->oxygenOffsets[i] = oPos - cPos;
		}

		transferAtomData(this->atomPositions, this->atomPositionsSize / 3, this->cAlphaIndices.data(), (unsigned int)this->cAlphaIndices.size(), this->oIndices.data(), (unsigned int)this->oIndices.size());
	}

	this->myHash = this->lastHash + this->hashOffset;
	agdc->SetDataHash(this->myHash);

	flatten();

	// compute the new bounding box
	vislib::math::Cuboid<float> newbb;
	if (mdc->AtomCount() != 0) {
		vislib::math::Vector<float, 3> p(&atomPositions[0]);
		float r = atomRadii[0];
		newbb = vislib::math::Cuboid<float>(p[0] - r, p[1] - r, p[2] - r, p[0] + r, p[1] + r, p[2] + r);
	}

	for (unsigned int i = 1; i < mdc->AtomCount(); i++) {
		vislib::math::Vector<float, 3> p(&atomPositions[i * 3]);
		float r = atomRadii[i];
		vislib::math::Cuboid<float> b(p[0] - r, p[1] - r, p[2] - r, p[0] + r, p[1] + r, p[2] + r);
		newbb.Union(b);
	}

	newbb.Grow(3.0f);

	this->boundingBox.Union(newbb);
	agdc->AccessBoundingBoxes().SetObjectSpaceBBox(this->boundingBox);
	agdc->AccessBoundingBoxes().SetObjectSpaceClipBox(this->boundingBox);

	return true;
}

/*
 *	SecStructFlattener::getPlaneData
 */
bool SecStructFlattener::getPlaneData(core::Call& call) {
	PlaneDataCall * pdc = dynamic_cast<PlaneDataCall*>(&call);
	if (pdc == NULL) return false;

	vislib::math::Vector<float, 3> pointVector;
	vislib::math::Vector<float, 3> normal;
	pointVector = this->arbPlaneCenterParam.Param<param::Vector3fParam>()->Value();
	vislib::math::Point<float, 3> point(pointVector.GetX(), pointVector.GetY(), pointVector.GetZ());

	switch (this->flatPlaneMode.Param<param::EnumParam>()->Value()) {
		case XY_PLANE:
			normal = vislib::math::Vector<float, 3>(0.0f, 0.0f, 1.0f);
			break;
		case XZ_PLANE:
			normal = vislib::math::Vector<float, 3>(0.0f, 1.0f, 0.0f);
			break;
		case YZ_PLANE:
			normal = vislib::math::Vector<float, 3>(1.0f, 0.0f, 0.0f);
			break;
		case LEAST_COMMON:
			normal = this->mainDirections[2];
			normal.Normalise();
			break;
		case ARBITRARY:
			normal = this->arbPlaneNormalParam.Param<param::Vector3fParam>()->Value();
			normal.Normalise();
			break;
		default:
			normal = vislib::math::Vector<float, 3>(0.0f, 0.0f, 1.0f);
			break;
	}

	this->currentPlane = vislib::math::Plane<float>(point, normal);
	pdc->SetPlaneData(&this->currentPlane);

	return true;
}

/*
 *	SecStructFlattener::getPlaneExtent
 */
bool SecStructFlattener::getPlaneExtent(core::Call& call) {
	PlaneDataCall * pdc = dynamic_cast<PlaneDataCall*>(&call);
	if (pdc == NULL) return false;

	pdc->SetPlaneCnt(1); // always one plane
	pdc->SetDataHash(this->planeHash);

	return true;
}

/*
 *	SecStructFlattener::onPlayToggleButton
 */
bool SecStructFlattener::onPlayToggleButton(param::ParamSlot& p) {
	param::BoolParam *bp = this->playParam.Param<param::BoolParam>();
	bp->SetValue(!bp->Value());
	bool play = bp->Value();
	return true;
}