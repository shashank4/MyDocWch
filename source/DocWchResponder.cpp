//========================================================================================
//  
//  $File: //depot/devtech/nevis/plugin/source/sdksamples/docwatch/DocWchResponder.cpp $
//  
//  Owner: Adobe Developer Technologies
//  
//  $Author: gmatthew $
//  
//  $DateTime: 2013/06/18 15:15:50 $
//  
//  $Revision: #1 $
//  
//  $Change: 853167 $
//  
//  Copyright 1997-2010 Adobe Systems Incorporated. All rights reserved.
//  
//  NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance 
//  with the terms of the Adobe license agreement accompanying it.  If you have received
//  this file from a source other than Adobe, then your use, modification, or 
//  distribution of it requires the prior written permission of Adobe.
//  
//========================================================================================

#include "VCPlugInHeaders.h"

#include <string>
#include <sstream>
#include <fstream>
#include<stdlib.h>

//#include <iostream>

using namespace std;
// Interface includes:
#include "IClassIDData.h"
#include "isignalmgr.h"
#include "IDocumentSignalData.h"
#include "IObserver.h"
#include "ISubject.h"
#include "INewStorySignalData.h"
#include "IUIDData.h"
#include "ITextModel.h"  
#include "ITextTarget.h"
#include "ISpreadList.h"
#include "ISpread.h"
#include "IGraphicFrameData.h"
#include "IGeometry.h"
#include "IGraphicsPort.h"
#include "IPMStream.h"
#include "IGeometryFacade.h"

#include "IDrawingStyle.h"
#include "IComposeScanner.h"
#include "IStyleGroupManager.h"
#include "IStyleInfo.h"
#include "IStyleNameTable.h"
#include "IWorkspace.h"
#include "IActiveContext.h"
#include "IAttributeStrand.h"
#include "IMultiColumnTextFrame.h"
#include "ITextFrameColumn.h"
#include "IHierarchy.h"
#include "IGeometryFacade.h"

#include "ISnippetExport.h"
#include "IScript.h"
#include "IScriptUtils.h"
#include "IDOMElement.h"
#include "IK2ServiceRegistry.h"
#include "ISelectionUtils.h"
#include "IExportProvider.h"
#include "IK2ServiceProvider.h"

#include "IPageItemTypeUtils.h"
#include "ILinkObject.h"
#include "ILinkManager.h"
#include "ILink.h"
#include "ILinkResource.h"



// Implementation includes:
#include "CreateObject.h"
#include "CResponder.h"
#include "PMString.h"
#include "CAlert.h"
#include "SelectionObserver.h"
#include "PMRect.h"
#include "CGraphicFrameShape.h"
#include "PMReal.h"
#include "IDFile.h"
#include "StreamUtil.h"
#include "json.h"
#include "FileUtils.h"
#include "IDFile.h"
#include "StreamUtil.h"
#include "ScriptingDefs.h"
#include "AppPrefsListElementTypes.h"
#include "TransformUtils.h"
#include "SnapshotUtils.h"


#include "DocWchUtils.h"
#include "DocWchID.h"

//------------------------------------------




int flag = 0;
int storyFlag = 0;
int totalTextModelLength = 0;

class DocWchResponder : public CResponder
{
public:
	DocWchResponder(IPMUnknown* boss);
	virtual void Respond(ISignalMgr* signalMgr);
	void attachDetachActiveSelectionObserver(UIDRef doc, bool16 bAttach);

private:
	ErrorCode attachDetachStoryObserver(UIDRef storyUIDRef, bool16 bAttach);
	void documentTraversal(UIDRef docRef, bool16 val);
	json::Object GetCoordObject(PMRect r);
	json::Object charParStyle(ITextModel* textModel);
	void getParaStyleInformation(IAttributeStrand* xyz, IWorkspace* theWS, int32 start, int32 count, json::Array & paraStyleArray);
	void getCaraStyleInformation(IAttributeStrand* icastrand, IWorkspace* theWS, int32 start, int32 count, json::Array & caraStyleArray);
	std::string getTextFrameData(ITextModel* textModel);
	json::Object getSpreadDimension(PMRect node);
	void getTextFrameDimension(IMultiColumnTextFrame* MCF, json::Object & frameObject, PMMatrix pb2page);
	void generateTextContentFileGetStyle(IMultiColumnTextFrame* MCF, json::Object & frameObject, json::Object& dataObject, std::string frameUIDStr);
	void ExportTextStyles();
	void exportSelectionHTML(IDocument* doc);
	void DocWchResponder::setItemToSnapshot(const UIDRef& itemToSnapshot);
};


CREATE_PMINTERFACE(DocWchResponder, kDocWchResponderImpl)


DocWchResponder::DocWchResponder(IPMUnknown* boss) :CResponder(boss){}

void DocWchResponder::Respond(ISignalMgr* signalMgr)
{

	ServiceID serviceTrigger = signalMgr->GetServiceID();
	//CAlert::InformationAlert("--------------------000------------------");
	//PMString xyz("515");//New Story creation serviceID
	//PMString abc;
	//abc.AppendNumber(serviceTrigger.Get());
	//CAlert::InformationAlert(abc);
	//Check if new story creation is there or not.
	//CAlert::InformationAlert("Before If.........");
	//if(abc == xyz)
	//if(flag==1)
	//{
	//	CAlert::InformationAlert("New Doc Start///////////////////");
	//if(serviceTrigger.Get()==kNewStorySignalResponderService)
	//{
	//	CAlert::InformationAlert("New story is created - before attach call");
	//	InterfacePtr<INewStorySignalData> iNewStorySignalData(signalMgr, UseDefaultIID());
	//	UIDRef story = iNewStorySignalData->GetTextModel();
	//	attachDetachStoryObserver(story,kTrue);
	//	CAlert::InformationAlert("New story is created - after attach call");
	//}
	//DocWchUtils::DwAlert(doc, kNewStoryCreateSignalStringKey);
	//	InterfacePtr<IDocumentSignalData> data(signalMgr, UseDefaultIID());
	//InterfacePtr<IDocument> iDocument(data->GetDocument(), UseDefaultIID());
	//InterfacePtr<ITextModel> iDocument(IID_ITEXTMODEL,UseDefaultIID);
	// attach the new/delete story observer
	//		InterfacePtr<IObserver> iNewDeleteStoryObserver(signalMgr,IID_IMYOBSERVER);
	//	if (iNewDeleteStoryObserver == nil){
	//	ASSERT_FAIL("GTTxtEdtResponder::Respond - document has no story create/delete observer in responder?");
	//}
	//iNewDeleteStoryObserver->AutoAttach();
	//}
	//}
	//if(serviceTrigger.Get()==kAfterNewDocSignalResponderService)
	//{
	//	flag=1;
	//}
	// Exit if the responder should do nothing
	//if (DocWchUtils::QueryDocResponderMode() != kTrue && DocWchUtils::QueryTextResponderMode()!=kTrue)
	//	return;
	// Get a UIDRef for the document.  It will be an invalid UIDRef
	// for BeforeNewDoc, BeforeOpenDoc, AfterSaveACopy, and AfterCloseDoc because
	// the document doesn't exist at that point.
	/*
	if(serviceTrigger.Get()==kAfterNewDocSignalResponderService)
	{
	flag=1;
	}

	if(flag==1)
	{*/
	switch (serviceTrigger.Get())
	{
		/*case kBeforeNewDocSignalResponderService:
		{
		InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
		UIDRef doc = docData->GetDocument();
		DocWchUtils::DwAlert(doc, kBeforeNewDocSignalStringKey);
		break;
		}
		case kDuringNewDocSignalResponderService:
		{
		InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
		UIDRef doc = docData->GetDocument();
		DocWchUtils::DwAlert(doc, kDuringNewDocSignalStringKey);
		break;
		}*/

	case kAfterNewDocSignalResponderService:
	{
		InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
		UIDRef doc = docData->GetDocument();

		//attachDetachActiveSelectionObserver(doc,kTrue);

		/*
		if(storyFlag==1)
		{
		//InterfacePtr<INewStorySignalData> iNewStorySignalData(signalMgr, UseDefaultIID());
		//UIDRef story = iNewStorySignalData->GetTextModel();
		CAlert::InformationAlert("B4 textTarget");
		InterfacePtr<ITextTarget> textTarget(this, UseDefaultIID());
		//InterfacePtr<ITextModel> textModel(textTarget->QueryTextModel ());
		UIDRef story=textTarget->GetTextModel();

		attachDetachActiveSelectionObserver(story,kTrue);

		//InterfacePtr<SelectionSuit> iMyObserver(story,IID_IMYACTIVESELECTIONOBSERVER);
		//iMyObserver->AutoAttach();


		}*/
		break;
	}
	/*

	case kBeforeOpenDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kBeforeOpenDocSignalStringKey);
	break;
	}
	case kDuringOpenDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kDuringOpenDocSignalStringKey);
	break;
	}
	case kAfterOpenDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kAfterOpenDocSignalStringKey);
	break;
	}
	case kAfterSaveDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kBeforeSaveDocSignalStringKey);
	break;
	}
	case kBeforeSaveDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	CAlert::InformationAlert("--------------------111------------------");
	this->documentTraversal(doc,kTrue);
	//this->attachDetachActiveSelectionObserver(doc, kTrue);
	break;
	}

	case kBeforeSaveAsDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	//DocWchUtils::DwAlert(doc, kBeforeSaveAsDocSignalStringKey);
	//CAlert::InformationAlert("--------------------111------------------");
	CAlert::InformationAlert("kBeforeSaveAsDocSignalResponderService");


	break;
	}
	*/

	case kAfterSaveAsDocSignalResponderService:
	{
		InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
		UIDRef doc = docData->GetDocument();
		//DocWchUtils::DwAlert(doc, kAfterSaveAsDocSignalStringKey);
		this->documentTraversal(doc, kTrue);

		InterfacePtr<IDocument> document(doc, UseDefaultIID());
		if (!document)	{ CAlert::InformationAlert("document failed");	break; }
		this->exportSelectionHTML(document);
		break;
	}/*
	case kBeforeSaveACopyDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kBeforeSaveACopyDocSignalStringKey);
	break;
	}
	case kDuringSaveACopyDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kDuringSaveACopyDocSignalStringKey);
	break;
	}
	case kAfterSaveACopyDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kAfterSaveACopyDocSignalStringKey);
	break;
	}
	case kBeforeRevertDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kBeforeRevertDocSignalStringKey);
	break;
	}
	case kAfterRevertDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kAfterRevertDocSignalStringKey);
	break;
	}
	case kBeforeCloseDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kBeforeCloseDocSignalStringKey);
	break;
	}
	case kAfterCloseDocSignalResponderService:
	{
	InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
	UIDRef doc = docData->GetDocument();
	DocWchUtils::DwAlert(doc, kAfterCloseDocSignalStringKey);
	break;
	}

	case kNewStorySignalResponderService:
	{
	storyFlag=1;
	//CAlert::InformationAlert("New story is created - before attach call");
	InterfacePtr<INewStorySignalData> iNewStorySignalData(signalMgr, UseDefaultIID());
	UIDRef story = iNewStorySignalData->GetTextModel();
	//attachDetachStoryObserver(story,kTrue);
	//CAlert::InformationAlert("New story is created - after attach call");
	break;
	}
	default:
	CAlert::InformationAlert("In Default");
	break;
	*/

	}
}

/*
ErrorCode DocWchResponder::attachDetachStoryObserver(UIDRef storyUIDRef, bool16 bAttach)
{
//CAlert::InformationAlert("In attachDetachActiveSelectionObserver");
ErrorCode status = kFailure;
do{

InterfacePtr<IObserver> iMyObserver(storyUIDRef,IID_IMYOBSERVER);
bAttach ? iMyObserver->AutoAttach() :iMyObserver->AutoDetach();
status = kSuccess;
}while (kFalse);
return status;
}
*/

void DocWchResponder::attachDetachActiveSelectionObserver(UIDRef doc, bool16 bAttach)
{

	ErrorCode status = kFailure;
	do{
		InterfacePtr<IObserver> iMyActiveSelectionObserver(doc, IID_IMYACTIVESELECTIONOBSERVER);
		iMyActiveSelectionObserver->AutoAttach();
		status = kSuccess;
	} while (kFalse);
}

/*


// Find the text model and range of text selected from
// the text target.
InterfacePtr<ITextTarget> textTarget(this, UseDefaultIID());
if (textTarget == nil) {
break;
}
result = UIDList(textTarget->GetTextModel().GetDataBase());
InterfacePtr<ITextModel> textModel(textTarget->QueryTextModel ());
if (textModel == nil) {
break;
}
InterfacePtr<ITextFocus> textFocus(textTarget->QueryTextFocus());
if (textFocus == nil) {
break;
}

*/


void DocWchResponder::documentTraversal(UIDRef docRef, bool16 bAttach) //doc traverse + call to chara para style 
{
	ofstream outfile;
	outfile.open("D:\\DocWch\\GeneratedFiles\\json.txt");
	int graphicFrameCount = 0, textFrameCount = 0;

	do
	{
		json::Object wrapper;
		json::Object dataObject;

		if (!docRef)	           { CAlert::InformationAlert("docRef failed");		break; }

		InterfacePtr<IDocument> document(docRef, UseDefaultIID());
		if (!document)	{ CAlert::InformationAlert("document failed");	break; }

		PMString docName;
		document->GetName(docName);

		std::string documentName = docName.GetPlatformString();
		json::Object docObject;
		docObject["DocumentName"] = documentName;

		IDataBase* database = ::GetDataBase(document);
		if (!database)   { CAlert::InformationAlert("database failed");	break; }

		InterfacePtr<ISpreadList> spreadList(document, UseDefaultIID());
		if (!spreadList) { CAlert::InformationAlert("spreadList failed");	break; }

		UIDList frameList(database);
		json::Array spreadListArray;

		int32 spreadCount = spreadList->GetSpreadCount();
		int32 spreadIndex;
		for (spreadIndex = 0; spreadIndex < spreadCount; spreadIndex++)
		{
			json::Object spreadObject;
			/*
			PMString spreadIndexNum;
			spreadIndexNum.AppendNumber(spreadIndex);
			std::string sprInd = spreadIndexNum.GetPlatformString();
			*/
			spreadObject["SpreadIndex"] = spreadIndex;

			//PMMatrix page2pb = ::InnerToPasteboardMatrix(pageGeometry);
			//PMMatrix pb2page = page2pb;
			//pb2page.Invert ();

			InterfacePtr<IGeometry> iSpreadGeometry(spreadList->QueryNthSpread(spreadIndex));
			if (!iSpreadGeometry) { CAlert::InformationAlert("iSpreadGeometry failed.");	break; }
			/*
			PMMatrix transformMatrix;

			InterfacePtr<IHierarchy> hierarchy(frameList.GetRef(spreadIndex), UseDefaultIID());
			if (hierarchy)
			{
			transformMatrix=TraceNodeGeometryAndTransform(hierarchy);
			}
			*/

			UIDRef spreadUIDRef(database, spreadList->GetNthSpreadUID(spreadIndex));

			PMMatrix page2pb3 = ::InnerToPasteboardMatrix(iSpreadGeometry);   //InnerToPasteboardMatrix
			PMMatrix pb2page3 = page2pb3;
			pb2page3.Invert();
			PMRect spreadPMRect;

			//PMRect spreadPMRect = iSpreadGeometry->GetStrokeBoundingBox();                                        //(pb2page);
			//PBPMRect spreadPMRect = Utils<Facade::IGeometryFacade>()->GetItemBounds( spreadUIDRef, Transform::PasteboardCoordinates(), Geometry::PathBounds());
			//::TransformInnerRectToPasteboard(iSpreadGeometry, &spreadPMRect);
			//::TransformInnerRectToSpread(iSpreadGeometry, &spreadPMRect);
			//json::Object spreadDim;
			//spreadDim=this->getSpreadDimension(spreadPMRect);
			//spreadObject["Spread Dim"]=spreadDim;
			/*
			json::Object spreadCoordObject;
			spreadCoordObject=this->GetCoordObject(spreadPMRect);
			spreadObject["Spread Dim"]=spreadCoordObject;
			*/


			InterfacePtr<ISpread> spread(spreadUIDRef, UseDefaultIID());
			if (!spread)			{ CAlert::InformationAlert("spread failed");		break; }


			int32 numberOfPages = spread->GetNumPages();
			PMString noPages;
			noPages.AppendNumber(numberOfPages);

			json::Array pageListArray;
			int32 pageIndex;
			for (pageIndex = 0; pageIndex < numberOfPages; pageIndex++)
			{
				json::Object pageObject;
				json::Array pageCoordArray;

				IGeometry * pageGeometry = spread->QueryNthPage(pageIndex);
				if (!pageGeometry) { CAlert::InformationAlert("pageGeometry failed.");	break; }

				const PMRect r1 = pageGeometry->GetStrokeBoundingBox();

				json::Object pageCoordObject;
				pageCoordObject = this->GetCoordObject(r1);
				//pageCoordArray.push_back(pageCoordObject);

				if (pageIndex == 0)
				{
					PMMatrix page2pbLocal = ::InnerToPasteboardMatrix(pageGeometry);   //InnerToPasteboardMatrix
					PMMatrix pb2pageLocal = page2pbLocal;
					pb2pageLocal.Invert();
					spreadPMRect = iSpreadGeometry->GetPathBoundingBox(pb2pageLocal);
				}

				PMMatrix page2pb = ::InnerToPasteboardMatrix(pageGeometry);   //InnerToPasteboardMatrix
				PMMatrix pb2page = page2pb;
				pb2page.Invert();



				UIDList pageItemList(database);
				spread->GetItemsOnPage(pageIndex, &pageItemList, kFalse, kFalse);
				// don't include the page object itself
				// don't include items that lie on the pasteboard because we only want what's on the page?


				int32 frameIndex;
				int32 pageItemListLength = pageItemList.Length();
				json::Array frameListArray;
				for (frameIndex = 0; frameIndex < pageItemListLength; frameIndex++)
				{
					json::Object frameObject;



					UIDRef pageItemRef = pageItemList.GetRef(frameIndex);

					UID frameUID = pageItemRef.GetUID();

					std::stringstream ss;
					ss << frameUID.Get();
					std::string frameUIDStr = ss.str();
					int frameUIDInt = atoi(frameUIDStr.c_str());
					frameObject["FrameIndex"] = frameUIDInt;

					Utils<IPageItemTypeUtils> pageItemTypeUtils;

					InterfacePtr<IGraphicFrameData> graphicFrameData(pageItemRef, UseDefaultIID());
					if (graphicFrameData)		{ graphicFrameCount++; }

					UID fromMultiColumnItemUID = graphicFrameData->GetTextContentUID();
					InterfacePtr<IMultiColumnTextFrame> MCF(database, fromMultiColumnItemUID, UseDefaultIID());
					if (MCF != nil)
					{
						this->getTextFrameDimension(MCF, frameObject, pb2page);
						this->generateTextContentFileGetStyle(MCF, frameObject, dataObject, frameUIDStr);
					}
					else
					{
						InterfacePtr<IGeometry> fGeometry(pageItemRef, UseDefaultIID());
						if (!fGeometry)	{ CAlert::InformationAlert("fGeometry failed");	break; }

						json::Object frameCoordObject;
						PMRect r = fGeometry->GetPathBoundingBox(pb2page);  //GetStrokeBoundingBox(pb2page);
						frameCoordObject = this->GetCoordObject(r);
						frameObject["FrameCoord"] = frameCoordObject;
					}

					//------------------------------------------------/-/
					if (pageItemTypeUtils->IsGraphicFrame(pageItemRef))
					{
						InterfacePtr<IHierarchy> frameHierarchy(pageItemRef, UseDefaultIID());
						ASSERT(frameHierarchy);
						if (!frameHierarchy)
						{
							break;
						}
						InterfacePtr<IHierarchy> childHierarchy(frameHierarchy->QueryChild(0));
						if (!childHierarchy)
						{
							break;
						}
						// If we're on a placed image we should have a data link to source item
						InterfacePtr<ILinkObject> iLinkObject(childHierarchy, UseDefaultIID());
						// get the link for this object

						IDataBase* iDataBase = ::GetDataBase(childHierarchy);
						InterfacePtr<ILinkManager> iLinkManager(iDataBase, iDataBase->GetRootUID(), UseDefaultIID());

						ILinkManager::QueryResult linkQueryResult;

						if (iLinkManager->QueryLinksByObjectUID(::GetUID(childHierarchy), linkQueryResult))
						{
							ILinkManager::QueryResult::const_iterator iter = linkQueryResult.begin();

							InterfacePtr<ILink> iLink(iDataBase, *iter, UseDefaultIID());
							if (iLink != nil)
							{
								InterfacePtr<ILinkResource> iLinkResource(iLinkManager->QueryResourceByUID(iLink->GetResource()));
								if (iLinkResource != nil)
								{
									PMString datalinkPath = iLinkResource->GetLongName(false);
									CAlert::InformationAlert("datalinkPath::" + datalinkPath);
									
									ofstream imgfile;
									imgfile.open("D:\\DocWch\\GeneratedFiles\\img.txt");

									std::string imagePathStr = datalinkPath.GetPlatformString();
									std::string str = imagePathStr;
									char ch = '\\';
									for (int i = 0; i < str.length(); i++) 
									{
										if (str[i] == '\\')
										{
											//CAlert::InformationAlert("Inside the IF...wow");
											//str[i] = strcat(str[i], ch);
											str[i] += '\\';
											i++;
											//str[i] = "\\\\";
										}
									}
									////// PROBLEM IS WITH PATH STRING.... C:\ABC<----->C:\\ABC..... IF YOU DONE THIS THEN OTHER CODE IS READY.
									imgfile << endl << imagePathStr << endl << str;

									std::ifstream image;
									image.open("d:\\try.jpg", ios::in | ios::binary); // input file   CHECK PATH...I HAVE CHANGED IT...CODE WORKS FINE IN CODEBLOCK..
									
									std::ostringstream oss; // output to string
									

									int len;
									char buf[1024];
									while ((len = image.readsome(buf, 1024)) > 0)
									{
										oss.write(buf, len);
									}

									std::string data = oss.str();
									
									//PMString datastr = data;
									//CAlert::InformationAlert("byte Stream is:" + datastr);
									/*
									image.seekg(0, ios::end);
									int n = image.tellg();
									image.seekg(0, ios::beg);

									char* res = new char[n];
									for (int i = 0; i < n; i++)
										res[i] = '5';

									bool bit = image.eof();

									image.read(res, n);
									*/
									//ifstream fl(name);
									/*
									image.seekg(0, ios::end);
									size_t len = image.tellg();
									char *ret = new char[len];
									image.seekg(0, ios::beg);
									image.read(ret, len);
									image.close();
									std::string data = ret;
									*/
									
									imgfile <<endl<< data;
									imgfile.close();
									
									
									//dataObject["frameUIDImage"] = data;
								}
							}
						}
					}

					//---------------------------------------------------------


					frameListArray.push_back(frameObject);
					frameObject.Clear();

				}
				/*
				PMString pageIndexNum;
				pageIndexNum.AppendNumber(pageIndex);
				std::string pageIndexStr = pageIndexNum.GetPlatformString();
				*/

				pageObject["PageIndex"] = pageIndex;
				pageObject["Frames"] = frameListArray;
				pageObject["PageCoord"] = pageCoordObject;
				pageListArray.push_back(pageObject);

			}

			json::Object spreadCoordObject;
			spreadCoordObject = this->getSpreadDimension(spreadPMRect);
			spreadObject["SpreadDim"] = spreadCoordObject;

			spreadObject["Pages"] = pageListArray;
			spreadListArray.push_back(spreadObject);
		}
		docObject["Spreads"] = spreadListArray;

		wrapper["Structure"] = docObject;
		wrapper["Data"] = dataObject;

		std::string serialized_json_string = json::Serialize(wrapper);
		outfile << endl << serialized_json_string << endl;
		outfile.close();
	} while (kFalse);

}


void DocWchResponder::getTextFrameDimension(IMultiColumnTextFrame* MCF, json::Object& frameObject, PMMatrix pb2page)
{
	InterfacePtr<IFrameList> frameList(MCF->QueryFrameList());

	for (int32 i = 0; i < frameList->GetFrameCount(); i++)
	{

		InterfacePtr<ITextFrameColumn> textFrameColumn(frameList->QueryNthFrame(i));
		if (textFrameColumn != nil)
		{
			InterfacePtr<IGeometry> textFrameGeometry(textFrameColumn, UseDefaultIID());
			if (textFrameGeometry != nil)
			{
				json::Object frameCoordObject;
				PMRect textFrameBounds = textFrameGeometry->GetPathBoundingBox(pb2page);
				::TransformInnerRectToPasteboard(textFrameGeometry, &textFrameBounds);

				frameCoordObject = this->GetCoordObject(textFrameBounds);
				frameObject["FrameCoord"] = frameCoordObject;
			}
			else
			{
				CAlert::InformationAlert("textFrameGeometry failed.");
			}
		}
		else
		{
			CAlert::InformationAlert("ITextFrameColumn failed.");
		}
	}


}

void DocWchResponder::generateTextContentFileGetStyle(IMultiColumnTextFrame* MCF, json::Object& frameObject, json::Object& dataObject, std::string frameUIDStr)
{
	InterfacePtr<ITextModel> textModel(MCF->QueryTextModel());
	if (textModel)
	{
		dataObject[frameUIDStr] = this->getTextFrameData(textModel);  //spreadIndexStr+pageIndexStr+frameIndexStr  json::Object temp

		json::Object frameStyleObject;

		frameStyleObject = this->charParStyle(textModel);
		frameObject["FrameContentStyle"] = frameStyleObject;
	}
}

json::Object DocWchResponder::GetCoordObject(PMRect r)
{

	json::Object coordObject;

	PMPoint rb = r.RightBottom();
	PMReal rbx = rb.X();		PMString rbx1;		rbx1.AppendNumber(rbx);		int rbxInt = rbx1.GetAsNumber();
	PMReal rby = rb.Y();		PMString rby1;		rby1.AppendNumber(rbx);		int rbyInt = rby1.GetAsNumber();

	PMPoint tl = r.LeftTop();
	PMReal tlx = tl.X();		PMString tlx1;		tlx1.AppendNumber(tlx);		int tlxInt = tlx1.GetAsNumber();
	PMReal tly = tl.Y();		PMString tly1;		tly1.AppendNumber(tly);		int tlyInt = tly1.GetAsNumber();

	PMPoint rt = r.RightTop();
	PMReal rtx = rt.X();		PMString rtx1;		rtx1.AppendNumber(rtx);		int rtxInt = rtx1.GetAsNumber();
	PMReal rty = rt.Y();		PMString rty1;		rty1.AppendNumber(rty);		int rtyInt = rty1.GetAsNumber();

	PMPoint lb = r.LeftBottom();
	PMReal lbx = lb.X();		PMString lbx1;		lbx1.AppendNumber(lbx);		int lbxInt = lbx1.GetAsNumber();
	PMReal lby = lb.Y();		PMString lby1;		lby1.AppendNumber(lby);		int lbyInt = lby1.GetAsNumber();

	coordObject["TopLeft-X"] = tlxInt;
	coordObject["TopLeft-Y"] = tlyInt;

	coordObject["RightTop-X"] = rtxInt;
	coordObject["RightTop-Y"] = rtyInt;

	coordObject["RightBottom-X"] = rbxInt;
	coordObject["RightBottom-Y"] = rbyInt;

	coordObject["LeftBottom-X"] = lbxInt;
	coordObject["LeftBottom-Y"] = lbyInt;

	return coordObject;
}

json::Object DocWchResponder::charParStyle(ITextModel* textModel)
{
	json::Object frameStyleObject;
	do
	{
		/*//IDocument *doc = GetExecutionContextSession()->GetActiveContext()->GetContextDocument();
		InterfacePtr<IDocument> document(docRef, UseDefaultIID());			if(!document)	{CAlert::InformationAlert("document failed");	break;	}
		UIDRef ws = document->GetDocWorkSpace();
		InterfacePtr<IStyleGroupManager> styleNameTable(ws, IID_IPARASTYLEGROUPMANAGER);

		/*
		PMString styleName("someStyle");
		UID paraStyleUID = paraStyleNameTable->FindByName(styleName);
		if(paraStyleUID != kInvalidUID)
		{
		//We found the style
		UIDRef styleRef(doc->GetDataBase(), paraStyleUID);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//UIDRef wsRef=document->GetDocWorkSpace();
		//InterfacePtr<IWorkspace> ws(document->GetDocWorkSpace(), UseDefaultIID());
		//InterfacePtr<IWorkspace> ws(GetExecutionContextSession()->QueryWorkspace());
		IActiveContext *context = GetExecutionContextSession()->GetActiveContext();

		IWorkspace* ws = context->GetContextWorkspace();
		if(!ws)		{	CAlert::InformationAlert("ws failed");	break;	}


		InterfacePtr<IStyleNameTable> styleNameTable(ws, IID_IPARASTYLENAMETABLE);//ICHARSTYLENAMETABLE_OBSOLETE
		//InterfacePtr<IStyleGroupManager> styleNameTable(document->GetDocWorkSpace(), IID_ICHARSTYLEGROUPMANAGER);
		if(!styleNameTable)		{	CAlert::InformationAlert("styleNameTable failed");	break;	}

		UID rootStyleUID=styleNameTable->GetRootStyleUID();
		/*
		int32 styleNum=styleNameTable->GetNumStyles();
		PMString styleNumString;
		styleNumString.AppendNumber(styleNum);
		CAlert::InformationAlert("paraStyle count is:"+styleNumString);

		for(int32 i=0;i<styleNum;i++)
		{

		UID styleUID=styleNameTable->GetNthStyle(i);

		//UID rootStyleUID=styleNameTable->GetRootStyleUID();
		//InterfacePtr<IStyleInfo> styleInfo(database,styleUID, UseDefaultIID());

		InterfacePtr<IStyleInfo> styleInfo(::GetDataBase(styleNameTable),rootStyleUID,UseDefaultIID());
		if(!styleInfo)		{	CAlert::InformationAlert("styleInfo failed");	break;	}
		/*
		bool16 res=styleInfo->IsParagraphStyle();
		if(res==kTrue)
		{
		CAlert::InformationAlert("IsParagraphStyle :");
		}
		else
		{
		CAlert::InformationAlert("NO .......IsParagraphStyle :");
		}

		PMString paraStyleName=styleInfo->GetName();
		CAlert::InformationAlert("paraStyleName is:"+paraStyleName);
		//}
		*/
		int32 start = 0;
		int32 count = 0, count2 = 0; // number of chars from start that have this style

		totalTextModelLength = textModel->TotalLength();

		InterfacePtr<IAttributeStrand> ipastrand((IAttributeStrand*)(textModel->QueryStrand(kParaAttrStrandBoss, IID_IATTRIBUTESTRAND)));
		if (!ipastrand)					{ CAlert::InformationAlert("ipastrand Failed");					break; }

		InterfacePtr<IAttributeStrand> icastrand((IAttributeStrand*)(textModel->QueryStrand(kCharAttrStrandBoss, IID_IATTRIBUTESTRAND)));
		if (!icastrand)					{ CAlert::InformationAlert("icastrand Failed");					break; }

		IActiveContext* activeContext = GetExecutionContextSession()->GetActiveContext();
		if (!activeContext)				{ CAlert::InformationAlert("activeContext Failed");				break; }

		IWorkspace * theWS = activeContext->GetContextWorkspace();

		json::Array paraStyleArray;
		json::Array caraStyleArray;

		this->getParaStyleInformation(ipastrand, theWS, start, count, paraStyleArray);
		this->getCaraStyleInformation(icastrand, theWS, start, count, caraStyleArray);

		frameStyleObject["ParaStyle"] = paraStyleArray;
		frameStyleObject["CaraStyle"] = caraStyleArray;

	} while (kFalse);
	return frameStyleObject;
}

void DocWchResponder::getParaStyleInformation(IAttributeStrand* ipastrand, IWorkspace* theWS, int32 start, int32 count, json::Array & paraStyleArray)
{
	do
	{
		if (totalTextModelLength == start)
		{
			break;
		}

		json::Object styleInfoObject;
		start = start + count;
		int32 count2;

		int32 count;
		UID parastyle = ipastrand->GetStyleUID(start, &count2);
		count = count2;

		IActiveContext* activeContext = GetExecutionContextSession()->GetActiveContext();
		if (!activeContext)		{ CAlert::InformationAlert("activeContext Failed");	break; }

		InterfacePtr<IStyleInfo> paraStyleInfo(::GetDataBase(theWS), parastyle, UseDefaultIID());
		if (!paraStyleInfo)		{ CAlert::InformationAlert("paraStyleInfo failed");	break; }

		PMString startCount, totLen;
		startCount.AppendNumber(start);
		totLen.AppendNumber(totalTextModelLength);

		PMString charCount;
		charCount.AppendNumber(count);

		PMString paraStyleName = paraStyleInfo->GetName();
		PMString fromCharNumbers;
		fromCharNumbers.AppendNumber(start);
		//CAlert::InformationAlert("paraStyleName is:"+paraStyleName+"&&"+fromCharNumbers+" START: "+startCount+" TotLength:"+totLen);
		int32 toNum = start + count;
		PMString toCharNumbers;
		toCharNumbers.AppendNumber(toNum);

		std::string paraStyleNameString = paraStyleName.GetPlatformString();
		std::string fromString = fromCharNumbers.GetPlatformString();
		std::string toString = toCharNumbers.GetPlatformString();
		std::string charCountStr = charCount.GetPlatformString();

		styleInfoObject["ParaStyleName"] = paraStyleNameString;
		styleInfoObject["StyleStart"] = start;// fromString;
		styleInfoObject["CharCount"] = count;// charCountStr;
		if (count != 0)
			paraStyleArray.push_back(styleInfoObject);
		//textDataFile<<"paraStyleName is:"<<paraStyleNameString<<"\t||applied from char number:"<<fromString<<"\t|| to :"<<toString<<endl;
		this->getParaStyleInformation(ipastrand, theWS, start, count, paraStyleArray);

	} while (kFalse);
}

void DocWchResponder::getCaraStyleInformation(IAttributeStrand* icastrand, IWorkspace* theWS, int32 start, int32 count, json::Array & caraStyleArray)
{
	do
	{
		if (totalTextModelLength == start)
		{
			break;
		}

		json::Object styleInfoObject;
		start = start + count;
		int32 count2;

		int32 count;
		UID carastyle = icastrand->GetStyleUID(start, &count2);
		count = count2;

		IActiveContext* activeContext = GetExecutionContextSession()->GetActiveContext();
		if (!activeContext)		{ CAlert::InformationAlert("activeContext Failed");	break; }

		InterfacePtr<IStyleInfo> caraStyleInfo(::GetDataBase(theWS), carastyle, UseDefaultIID());
		if (!caraStyleInfo)		{ CAlert::InformationAlert("caraStyleInfo failed");	break; }

		PMString caraStyleName = caraStyleInfo->GetName();
		PMString fromCharNumbers;
		fromCharNumbers.AppendNumber(start);
		//CAlert::InformationAlert("caraStyleName is:"+caraStyleName+" && "+charNumbers);

		int32 toNum = start + count;
		PMString toCharNumbers;
		toCharNumbers.AppendNumber(toNum);

		PMString charCount;
		charCount.AppendNumber(count);

		std::string caraStyleNameString = caraStyleName.GetPlatformString();
		std::string fromString = fromCharNumbers.GetPlatformString();
		std::string toString = toCharNumbers.GetPlatformString();
		std::string charCountStr = charCount.GetPlatformString();

		styleInfoObject["CharaStyleName"] = caraStyleNameString;
		styleInfoObject["StyleStart"] = start;// fromString;
		styleInfoObject["CharCount"] = count;// charCountStr;

		if (count != 0)
			caraStyleArray.push_back(styleInfoObject);

		this->getCaraStyleInformation(icastrand, theWS, start, count, caraStyleArray);

	} while (kFalse);
}

std::string DocWchResponder::getTextFrameData(ITextModel* textModel)
{
	int32 totLength;
	WideString result;
	PMString fResult;
	totLength = textModel->TotalLength();
	TextIterator begin(textModel, 0);
	TextIterator end(textModel, totLength);
	for (TextIterator iter = begin; iter < end; iter++)
	{
		const UTF32TextChar characterCode = *iter;
		result.Append(characterCode);
	}
	fResult.Append(result);
	std::string textData = fResult.GetPlatformString();

	return textData;
}

json::Object DocWchResponder::getSpreadDimension(PMRect spreadPMRect)
{
	json::Object sprDim, refer;

	PMReal a = spreadPMRect.Top();		PMString top;		top.AppendNumber(a);		int topInt = top.GetAsNumber();
	PMReal b = spreadPMRect.Left();		PMString left;		left.AppendNumber(b);		int leftInt = left.GetAsNumber();
	PMReal c = spreadPMRect.Bottom();	PMString bottom;	bottom.AppendNumber(c);		int bottomInt = bottom.GetAsNumber();
	PMReal d = spreadPMRect.Right();	PMString right;		right.AppendNumber(d);		int rightInt = right.GetAsNumber();

	refer["Top"] = topInt;
	refer["Left"] = leftInt;
	refer["Bottom"] = bottomInt;
	refer["Right"] = rightInt;


	PMReal spreadHeight = spreadPMRect.Height();
	PMReal spreadWidth = spreadPMRect.Width();
	PMString sprHgt;	sprHgt.AppendNumber(spreadHeight);		int sprHgtInt = sprHgt.GetAsNumber();
	PMString sprWdt;	sprWdt.AppendNumber(spreadWidth);		int sprWdtInt = sprWdt.GetAsNumber();

	sprDim["SpreadHeight"] = sprHgtInt;
	sprDim["SpreadWidth"] = sprWdtInt;
	sprDim["SpreadReference"] = refer;

	return sprDim;
}
/*
void DocWchResponder::ExportTextStyles()    ///something went wrong...need css not xml
{
// Exporting all text styles is pretty simple.
// The thing to keep in mind is these styles may contain references to other
// objects (like swatches).  These other objects (if scriptable) will be
// brought along by the snippet architecture.

K2Vector<ScriptID>  idList;

idList.push_back(c_CharStyle);
idList.push_back(c_ParaStyle);


K2Vector<ScriptID>  c_id;
c_id.push_back(c_CharStyle);

PMString fullPath("D:\\DocWch\\GeneratedFiles\\styleExport.css");

IDFile targetFile(fullPath);


//InterfacePtr<IPMStream> stream(CreateStream(IPMStream::kWriter) );
InterfacePtr<IPMStream> stream(StreamUtil::CreateFileStreamWrite(targetFile, kOpenOut | kOpenTrunc, 'CSS', 'CWIE'));
if (!stream)
CAlert::InformationAlert(" stream kFailure");

ErrorCode result = Utils<ISnippetExport>()->ExportAppPrefs(stream, ISnippetExport::kInclude, c_id);


}
*/
void DocWchResponder::exportSelectionHTML(IDocument* doc)
{
	PMString fullPath("D:\\DocWch\\GeneratedFiles\\selectionExport.html");
	IDFile targetFile(fullPath);

	InterfacePtr<IK2ServiceRegistry> k2ServiceRegistry(GetExecutionContextSession(), UseDefaultIID());

	int32 exportProviderCount = k2ServiceRegistry->GetServiceProviderCount(kExportProviderService);
	bool found = kFalse;

	for (int32 exportProviderIndex = 0; exportProviderIndex < exportProviderCount; exportProviderIndex++)
	{
		InterfacePtr<IK2ServiceProvider> k2ServiceProvider(k2ServiceRegistry->QueryNthServiceProvider(kExportProviderService, exportProviderIndex));
		InterfacePtr<IExportProvider> exportProvider(k2ServiceProvider, IID_IEXPORTPROVIDER);
		exportProvider->ExportToFile(targetFile, doc, doc, "HTML", kSuppressUI);
	}
}

void DocWchResponder::setItemToSnapshot(const UIDRef& itemToSnapshot)
{
	const PMReal xScale = 1.0;
	const PMReal yScale = 1.0;
	const PMReal desiredRes = 36.0;
	SnapshotUtils* fSnapshotUtils;

	ErrorCode status = kFailure;
	PMString fullPath("D:\\DocWch\\GeneratedFiles\\imageStream.css");

	//IDFile jpegSysFile(fullPath);

	do {
		// make sure item is valid
		if (itemToSnapshot.ExistsInDB() == kFalse)
		{
			ASSERT(itemToSnapshot.ExistsInDB());
			break;
		}

		// Clean up first.
		//this->CleanUp();

		PMReal minRes = desiredRes;
		// create a new snapshot utils instance
		fSnapshotUtils = new SnapshotUtils(itemToSnapshot,
			xScale,     // X Scale
			yScale,     // Y Scale
			desiredRes,    // desired resolution of resulting snapshot
			minRes,    // minimum resolution of resulting snapshot
			0,     // Extend the bounds of the area to be drawn by the given amount of bleed
			IShape::kDrawFrameEdge,// Drawing flags (kPrinting suppresses drawing of margins and guides)
			kTrue,    // kTrue forces images and graphics to draw at full resolution, kFalse to draw proxies
			kFalse,    // kTrue to draw grayscale, kFalse to draw RGB
			kFalse);    // kTrue to add an alpha channel, kFalse no alpha channel



		if (fSnapshotUtils == nil)
		{
			CAlert::InformationAlert("fSnapshotUtils failed.");
			break;
		}


		// see if you can get an AGM image accessor out of this
		AGMImageAccessor* imageAccessor = fSnapshotUtils->CreateAGMImageAccessor();
		if (imageAccessor == nil)
		{
			CAlert::InformationAlert("Failed imageAccessor");
			break;
		}
		else
		{
			// we don't need this anymore so delete it
			CAlert::InformationAlert("Success imageAccessor");
			delete imageAccessor;
		}

		IDFile jpegSysFile(fullPath);
		uint32 mode = kOpenOut | kOpenTrunc;
		OSType fileType = 'JPEG';
		OSType creator = kAnyCreator;

		// create a stream to write out
		InterfacePtr<IPMStream> jpegStream(StreamUtil::CreateFileStreamWrite(jpegSysFile, mode, fileType, creator));
		if (jpegStream == nil)
		{
			CAlert::InformationAlert("jpegStream failed");
			break;
		}

		// export to JPEG
		status = fSnapshotUtils->ExportImageToJPEG(jpegStream);

		// if we got here, everything is good
		status = kSuccess;

	} while (false);

}