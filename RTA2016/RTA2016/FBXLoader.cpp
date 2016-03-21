#include "FBXLoader.h"
FBXLoader::FBXLoader()
{

}

FBXLoader::~FBXLoader()
{
}

void FBXLoader::ReadIn(const char * _fileName)
{
	fstream	fin;
	string filepath = _fileName;

	if (strlen(_fileName) != 0)
	{
		fin.open(filepath, std::ios_base::in);
	}

	if (fin.is_open())
	{
		while (!fin.eof())
		{
			char lineHeader[128];
			fin.getline(lineHeader, 128);

			char _11[5];
			char _21[5];
			char _31[5];
			char _41[5];
			char _12[5];
			char _22[5];
			char _32[5];
			char _42[5];
			char _13[5];
			char _23[5];
			char _33[5];
			char _43[5];
			char _14[5];
			char _24[5];
			char _34[5];
			char _44[5];

			fin.getline(_11, 5);
			fin.getline(_12, 5);
			fin.getline(_13, 5);
			fin.getline(_14, 5);
			fin.getline(_21, 5);
			fin.getline(_22, 5);
			fin.getline(_23, 5);
			fin.getline(_24, 5);
			fin.getline(_31, 5);
			fin.getline(_32, 5);
			fin.getline(_33, 5);
			fin.getline(_34, 5);
			fin.getline(_41, 5);
			fin.getline(_42, 5);
			fin.getline(_43, 5);
			fin.getline(_44, 5);


			XMFLOAT4X4 tempMat;

			tempMat._11 = (float)atof(_11);
			tempMat._21 = (float)atof(_21);
			tempMat._31 = (float)atof(_31);
			tempMat._41 = (float)atof(_41);
			tempMat._12 = (float)atof(_12);
			tempMat._22 = (float)atof(_22);
			tempMat._32 = (float)atof(_32);
			tempMat._42 = (float)atof(_42);
			tempMat._13 = (float)atof(_13);
			tempMat._23 = (float)atof(_23);
			tempMat._33 = (float)atof(_33);
			tempMat._43 = (float)atof(_43);
			tempMat._14 = (float)atof(_14);
			tempMat._24 = (float)atof(_24);
			tempMat._34 = (float)atof(_34);
			tempMat._44 = (float)atof(_44);


			m_filePaths.push_back((string)lineHeader);
			m_worldMatrices.push_back(tempMat);
		}
	}
	//m_filePaths.push_back("Teddy_Idle");
	//m_filePaths.push_back("Box_BindPose");
	m_filePaths.push_back("sphere");
	//m_filePaths.push_back("Box_BindPose");

}

void FBXLoader::FBXBinaryCheck()
{
	for (unsigned int i = 0; i < m_filePaths.size(); i++)
	{
		struct stat filePropertyB;
		struct stat filePropertyF;

		char tempB[135];
		strcpy_s(tempB, m_filePaths[i].c_str());
		strcat_s(tempB, ".tribal");
		char tempF[132];
		strcpy_s(tempF, m_filePaths[i].c_str());
		strcat_s(tempF, ".fbx");

		int retValB = stat(tempB, &filePropertyB);
		int retValF = stat(tempF, &filePropertyF);


		if (retValB != 0)
		{
			if (retValF == 0)
			{
				FBXBinaryConvert(tempF, tempB);
				--i;
				continue;
			}
			else
			{
				continue;
			}
		}

		if (retValB == 0)
		{
			time_t	updateTimeB;
			time_t	updateTimeF;

			//save last modified time to the file
			updateTimeB = filePropertyB.st_mtime;
			updateTimeF = filePropertyF.st_mtime;

			if (updateTimeB < updateTimeF)
			{
				FBXBinaryConvert(tempF, tempB);
			}
		}
		LoadBinary(tempB);
	}
}

void FBXLoader::FBXBinaryConvert(const char * _fileName, const char * _binName)
{
	// USED RTABase as base

	FbxManager* sdkManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ioSettings);

	FbxImporter* importer = FbxImporter::Create(sdkManager, "");
	bool importerStatus = importer->Initialize(_fileName, -1, sdkManager->GetIOSettings());
	if (!importerStatus)
	{
		printf("The call to the FbxImporter::Initialize() FAILED.\n");
		printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
		exit(-1);
	}

	scene = FbxScene::Create(sdkManager, "myScene");
	importer->Import(scene);
	importer->Destroy();  // Not sure I can call this here.

	//FbxGeometryConverter geometryConverter = FbxGeometryConverter(sdkManager);

	//if (geometryConverter.Triangulate(scene, true) == false)
	//{
	//	return false;
	//}

	// if (geometryConverter.SplitMeshesPerMaterial(scene, true) == false)
	//{
	//return false;
	//}

	vector<FbxNode*> fbxJoints;

	FbxNode* rootNode = scene->GetRootNode();

	vector<Mesh> meshes;

	for (int i = 0; i < scene->GetSrcObjectCount<FbxMesh>(); ++i)
	{
		FbxMesh* meshAttribute = scene->GetSrcObject<FbxMesh>(i);

		Mesh mesh;
		vector<unsigned int> controlPointIndices;

		if (LoadMesh(meshAttribute, mesh, controlPointIndices) == false)
		{
			return;
		}
		skeletonPTR = &mesh.GetSkeleton();
		skeletonNodes.clear();
		ProcessSkeletonHierarchy(rootNode);
		for (size_t j = 0; j < skeletonPTR->size(); j++)
		{
			XMStoreFloat4x4(&skeletonPTR->operator[](j).World, XMMatrixIdentity());
			XMStoreFloat4x4(&skeletonPTR->operator[](j).Local, XMMatrixIdentity());
			XMStoreFloat4x4(&skeletonPTR->operator[](j).GlobalBind, XMMatrixIdentity());
		}
		ProcessJointsAndAnimations(meshAttribute);
		/*for (unsigned int i = 0; i < skeletonPTR->size(); i++)
		{
		if (!skeletonNodes[i]->GetMesh())
		{
		continue;
		}
		ProcessJointsAndAnimations(skeletonNodes[i]);
		}*/

		//for (unsigned int i = 0; i < rootNode->GetChildCount(); i++)
		//{
		//	if (!rootNode->GetChild(i)->GetMesh())
		//	{
		//		continue;
		//	}
		//	ProcessJointsAndAnimations(skeletonNodes[i]);
		//}
		/*if (LoadTexture(meshAttribute, mesh) == false)
		{
		return;
		}*/
		mesh.GetSkeleton() = *skeletonPTR;
		meshes.push_back(mesh);
	}

	fstream bout;

	bout.open(_binName, std::ios_base::binary | std::ios_base::out);
	if (bout.is_open())
	{
		unsigned int numMeshes = meshes.size();
		bout.write((char*)&numMeshes, sizeof(unsigned int));
		for (unsigned int i = 0; i < numMeshes; i++)
		{
			unsigned int uniqueSize = meshes[i].GetVertices().size();
			unsigned int indiciesSize = meshes[i].GetIndices().size();
			unsigned int textNamesSize = meshes[i].GetTextureNames().size();
			unsigned int jointsSize = meshes[i].GetSkeleton().size();
			//unsigned int textsSize = meshes[i].GetTextures().size();
			string name = meshes[i].GetName();

			bout.write((char*)&name, 128);
			bout.write((char*)&uniqueSize, sizeof(unsigned int));
			bout.write((char*)&indiciesSize, sizeof(unsigned int));
			bout.write((char*)&textNamesSize, sizeof(unsigned int));
			bout.write((char*)&jointsSize, sizeof(unsigned int));

			vector<Mesh::UniqueMeshVertex> tempVerts = meshes[i].GetVertices();
			vector<Joint> tempJoints = meshes[i].GetSkeleton();
			for (unsigned int j = 0; j < uniqueSize; ++j)
			{
				bout.write((char*)&tempVerts[j], sizeof(Mesh::UniqueMeshVertex));
			}

			vector<unsigned int> tempInd = meshes[i].GetIndices();

			for (unsigned int j = 0; j < indiciesSize; ++j)
			{
				bout.write((char*)&tempInd[j], sizeof(unsigned int));
			}
			for (unsigned int j = 0; j < jointsSize; j++)
			{
				bout.write((char*)&tempJoints[j].World, sizeof(XMFLOAT4X4));
				bout.write((char*)&tempJoints[j].Local, sizeof(XMFLOAT4X4));
				bout.write((char*)&tempJoints[j].GlobalBind, sizeof(XMFLOAT4X4));
				bout.write((char*)&tempJoints[j].bDirty, sizeof(bool));
				bout.write((char*)&tempJoints[j].name, sizeof(128));
				bout.write((char*)&tempJoints[j].parentIndex, sizeof(unsigned int));
				bout.write((char*)&tempJoints[j].SkinWeight, sizeof(BlendingIndexWeightPair));
				unsigned int numFrames = tempJoints[j].frames.size();
				bout.write((char*)&numFrames, sizeof(unsigned int));
				for (unsigned int  k = 0; k < numFrames; k++)
				{
					bout.write((char*)&tempJoints[j].frames[k], sizeof(JointKeys));
				}
			}

		}

		bout.close();
	}

}

bool FBXLoader::LoadMesh(FbxMesh* meshAttribute, Mesh& mesh, vector<unsigned int>& controlPointIndices)
{
	// USED RTABase as base

	unsigned int polygonCount = meshAttribute->GetPolygonCount();
	unsigned int vertexID = 0;

	FbxVector4* controlPoints = meshAttribute->GetControlPoints();

	unsigned int controlPointCount = meshAttribute->GetControlPointsCount();

	for (unsigned int polygon = 0; polygon < polygonCount; ++polygon)
	{
		unsigned int polygonVertexCount = meshAttribute->GetPolygonSize(polygon);
		for (unsigned int polygonVertex = 0; polygonVertex < polygonVertexCount; ++polygonVertex)
		{
			Mesh::UniqueMeshVertex uniqueVert;
			unsigned int controlPointIndex = meshAttribute->GetPolygonVertex(polygon, polygonVertex);

			FbxVector4 translation = meshAttribute->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
			FbxVector4 rotation = meshAttribute->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
			FbxVector4 scaling = meshAttribute->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);
			FbxAMatrix matGeometry = FbxAMatrix(translation, rotation, scaling);

			FbxVector4 position;
			position = controlPoints[controlPointIndex].mData;
			position = matGeometry.MultT(position);

			uniqueVert.uVPos.x = (float)position.mData[0];
			uniqueVert.uVPos.y = (float)position.mData[1];
			uniqueVert.uVPos.z = (float)position.mData[2];

			// Get them texture coordinates
			unsigned int uvElementCount = meshAttribute->GetElementUVCount();
			for (unsigned int uvElevement = 0; uvElevement < uvElementCount; ++uvElevement)
			{
				FbxGeometryElementUV* geometryElementUV = meshAttribute->GetElementUV(uvElevement);
				FbxLayerElement::EMappingMode mappingMode = geometryElementUV->GetMappingMode();
				FbxLayerElement::EReferenceMode referenceMode = geometryElementUV->GetReferenceMode();

				int directIndex = -1;

				// eByControlPoint:  A single control point has a single uv
				// eByPolygonVertex:  A single control point can have multiple uv's

				if (mappingMode == FbxGeometryElement::eByControlPoint)
				{
					if (referenceMode == FbxGeometryElement::eDirect)
					{
						directIndex = controlPointIndex;
					}
					else if (referenceMode == FbxGeometryElement::eIndexToDirect)
					{
						directIndex = geometryElementUV->GetIndexArray().GetAt(controlPointIndex);
					}
				}
				else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (referenceMode == FbxGeometryElement::eDirect ||
						referenceMode == FbxGeometryElement::eIndexToDirect)
					{
						directIndex = meshAttribute->GetTextureUVIndex(polygon, polygonVertex);
					}
				}
				if (directIndex != -1)
				{
					FbxVector2 uv = geometryElementUV->GetDirectArray().GetAt(directIndex);

					uniqueVert.textCoord.u = (float)uv.mData[0];
					uniqueVert.textCoord.v = (float)uv.mData[1];
				}
			}

			// Get those Normals in here
			unsigned int normalElementCount = meshAttribute->GetElementNormalCount();
			for (unsigned int normalElement = 0; normalElement < normalElementCount; ++normalElement)
			{
				FbxGeometryElementNormal* geometryElementNormal = meshAttribute->GetElementNormal(normalElement);
				FbxLayerElement::EMappingMode mappingMode = geometryElementNormal->GetMappingMode();
				FbxLayerElement::EReferenceMode referenceMode = geometryElementNormal->GetReferenceMode();

				int directIndex = -1;

				if (mappingMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (referenceMode == FbxGeometryElement::eDirect)
					{
						directIndex = vertexID;
					}
					else if (referenceMode == FbxGeometryElement::eIndexToDirect)
					{
						directIndex = geometryElementNormal->GetIndexArray().GetAt(vertexID);
					}
				}

				if (directIndex != -1)
				{
					FbxVector4 norm = geometryElementNormal->GetDirectArray().GetAt(directIndex);

					uniqueVert.uVNorm.x = (float)norm.mData[0];
					uniqueVert.uVNorm.y = (float)norm.mData[1];
					uniqueVert.uVNorm.z = (float)norm.mData[2];
				}
			}

			vector<Mesh::UniqueMeshVertex>& uniqueVerts = mesh.GetVertices();

			unsigned int size = uniqueVerts.size();
			unsigned int i;
			for (i = 0; i < size; ++i)
			{
				if (uniqueVert == uniqueVerts[i])
				{
					break;
				}
			}

			if (i == size)
			{
				uniqueVerts.push_back(uniqueVert);
				controlPointIndices.push_back(controlPointIndex);
			}

			mesh.GetIndices().push_back(i);
			++vertexID;
		}
	}



	return true;

}

bool FBXLoader::LoadTexture(FbxMesh* meshAttribute, Mesh& mesh)
{
	return false;
}

void FBXLoader::LoadBinary(const char * _binName)
{

	vector<Mesh> meshes;


	fstream bin;
	if (strlen(_binName) != 0)
	{
		bin.open(_binName, std::ios_base::binary | std::ios_base::in);
	}

	if (bin.is_open())
	{
		unsigned int numMeshes;
		bin.read((char*)&numMeshes, sizeof(unsigned int));

		for (unsigned int i = 0; i < numMeshes; i++)
		{
			char name[128];
			unsigned int uniqueSize;
			unsigned int indiciesSize;
			unsigned int textNamesSize;
			unsigned int jointsSize;

			bin.read((char*)&name, 128);
			bin.read((char*)&uniqueSize, sizeof(unsigned int));
			bin.read((char*)&indiciesSize, sizeof(unsigned int));
			bin.read((char*)&textNamesSize, sizeof(unsigned int));
			bin.read((char*)&jointsSize, sizeof(unsigned int));

			Mesh tempMesh;
			tempMesh.GetName() = _binName;

			vector<Mesh::UniqueMeshVertex> tempVerts;
			tempVerts.resize(uniqueSize);

			for (unsigned int j = 0; j < uniqueSize; j++)
			{
				bin.read((char*)&tempVerts[j], sizeof(Mesh::UniqueMeshVertex));
			}

			vector<unsigned int> tempInd;
			tempInd.resize(indiciesSize);

			for (unsigned int j = 0; j < indiciesSize; j++)
			{
				bin.read((char*)&tempInd[j], sizeof(unsigned int));
			}

			vector<Joint> tempSkele;
			tempSkele.resize(jointsSize);

			for (unsigned int j = 0; j < jointsSize; j++)
			{
				bin.read((char*)&tempSkele[j].World, sizeof(XMFLOAT4X4));
				bin.read((char*)&tempSkele[j].Local, sizeof(XMFLOAT4X4));
				bin.read((char*)&tempSkele[j].GlobalBind, sizeof(XMFLOAT4X4));
				bin.read((char*)&tempSkele[j].bDirty, sizeof(bool));
				bin.read((char*)&tempSkele[j].name, sizeof(128));
				bin.read((char*)&tempSkele[j].parentIndex, sizeof(unsigned int));
				bin.read((char*)&tempSkele[j].SkinWeight, sizeof(BlendingIndexWeightPair));
				unsigned int numFrames;
				bin.read((char*)&numFrames, sizeof(unsigned int));
				tempSkele[j].frames.resize(numFrames);
				for (unsigned int k = 0; k < numFrames; k++)
				{
					bin.read((char*)&tempSkele[j].frames[k], sizeof(JointKeys));
				}
			}
			tempMesh.GetVertices() = tempVerts;
			tempMesh.GetIndices() = tempInd;
			tempMesh.GetSkeleton() = tempSkele;

			meshes.push_back(tempMesh);
		}
		bin.close();
	}
	Renderer::Objects.push_back(meshes);
}

void FBXLoader::ProcessSkeletonHierarchy(FbxNode* _rootNodeIn)
{
	int temp = _rootNodeIn->GetChildCount();
	for (int indexOfChild = 0; indexOfChild < _rootNodeIn->GetChildCount(); ++indexOfChild)
	{
		FbxNode* currentNode = _rootNodeIn->GetChild(indexOfChild);
		ProcessSkeletonHierarchyRecursively(currentNode, 0, -1);
	}
}

void FBXLoader::ProcessSkeletonHierarchyRecursively(FbxNode* _nodeIn, unsigned int _myIndex, int _parentIndexIn)
{
	if (_nodeIn->GetNodeAttribute() && _nodeIn->GetNodeAttribute()->GetAttributeType() && _nodeIn->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		string temp;
		Joint currJoint;
		currJoint.parentIndex = _parentIndexIn;
		temp = _nodeIn->GetName();
		strcpy_s(currJoint.name, temp.c_str());
		skeletonPTR->push_back(currJoint);
	}
	for (int i = 0; i < _nodeIn->GetChildCount(); ++i)
	{
		ProcessSkeletonHierarchyRecursively(_nodeIn->GetChild(i), skeletonPTR->size(), _myIndex);
	}
}

void FBXLoader::ProcessJointsAndAnimations(FbxMesh* _nodeIn)
{
	FbxMesh* currentMesh = _nodeIn;
	unsigned int numDeformers = currentMesh->GetDeformerCount();
	//FbxAMatrix geoTransform = GetGeometryTransformation(currentMesh); // Something the forums said to do for those random 1%

	for (unsigned int deformerIndex = 0; deformerIndex < numDeformers; ++deformerIndex)
	{
		FbxSkin* currentSkin = reinterpret_cast<FbxSkin*>(currentMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		//if (!currentSkin)
		//{
		//	continue;
		//}

		unsigned int numClusters = currentSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numClusters; ++clusterIndex)
		{
			FbxCluster* currentCluster = currentSkin->GetCluster(clusterIndex);
			string currentJoinName = currentCluster->GetLink()->GetName();
			unsigned int currentJointIndex = FindJointIndexUsingName(currentJoinName);
			FbxAMatrix local;
			FbxAMatrix world;
			FbxAMatrix globalBindposeInverseMatrix;

			currentCluster->GetTransformMatrix(local);
			currentCluster->GetTransformLinkMatrix(world);
			globalBindposeInverseMatrix = world.Inverse() * local;// *geoTransform;
			skeletonPTR->operator[](currentJointIndex).Local = fbxToFloatMatrix(local);
			skeletonPTR->operator[](currentJointIndex).World = fbxToFloatMatrix(world);
			skeletonPTR->operator[](currentJointIndex).GlobalBind = fbxToFloatMatrix(globalBindposeInverseMatrix);
			//skeletonPTR->operator[](currentJointIndex).Node = currentCluster->GetLink();

			unsigned int numIndecies = currentCluster->GetControlPointIndicesCount();
			for (unsigned int indeciesIndex = 0; indeciesIndex < numIndecies; ++indeciesIndex)
			{
				BlendingIndexWeightPair currentBlendingPair;
				currentBlendingPair.BlendingIndex = (float)currentJointIndex;
				currentBlendingPair.BlendingWeight = (float)currentCluster->GetControlPointWeights()[indeciesIndex];
				skeletonPTR->operator[](currentJointIndex).SkinWeight = currentBlendingPair;
			}
			FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>();
			if (!animStack)
				continue;
			FbxString stackName = animStack->GetName();
			FbxTakeInfo* take = scene->GetTakeInfo(stackName);
			FbxTime startFrame = take->mLocalTimeSpan.GetStart();
			FbxTime endFrame = take->mLocalTimeSpan.GetStop();

			for (FbxLongLong keyFrameIndex = startFrame.GetFrameCount(FbxTime::eFrames24); keyFrameIndex < endFrame.GetFrameCount(FbxTime::eFrames24); keyFrameIndex++)
			{
				FbxTime time;
				time.SetFrame(keyFrameIndex, FbxTime::eFrames24);
				JointKeys tempFrame;
				tempFrame.time = (float)time.GetSecondDouble();
				tempFrame.local = fbxToFloatMatrix(currentCluster->GetLink()->EvaluateGlobalTransform(time).Inverse());
				skeletonPTR->operator[](currentJointIndex).frames.push_back(tempFrame);
			}
		}
	}

}

FbxAMatrix FBXLoader::GetGeometryTransformation(FbxNode* _nodeIn)
{
	if (!_nodeIn)
	{
		throw std::exception("Null for mesh geometry");
	}

	const FbxVector4 lTranslation = _nodeIn->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lRotation = _nodeIn->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lScale = _nodeIn->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lTranslation, lRotation, lScale);
}

unsigned int FBXLoader::FindJointIndexUsingName(const string& _JointNameIn)
{
	for (unsigned int i = 0; i < skeletonPTR->size(); ++i)
	{
		Joint temp = skeletonPTR->operator[](i);
		if (temp.name == _JointNameIn)
		{
			return i;
		}
	}
	return 1;
}

XMFLOAT4X4 FBXLoader::fbxToFloatMatrix(FbxAMatrix& _matrixIn)
{
	XMFLOAT4X4 temp;
	//_matrixIn = _matrixIn.Transpose();

	for (unsigned int i = 0; i < 4; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			temp.m[i][j] = (float)_matrixIn.Get(i, j);
		}
	}

	return temp;
}