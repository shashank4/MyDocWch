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

#include "DocWchUtils.h"
#include "DocWchID.h"

//------------------------------------------
#include "TransformUtils.h"




int flag=0;
int storyFlag=0;
int totalTextModelLength=0;

class DocWchResponder : public CResponder
{
public:
	DocWchResponder(IPMUnknown* boss);
	virtual void Respond(ISignalMgr* signalMgr);
	void attachDetachActiveSelectionObserver(UIDRef doc, bool16 bAttach);

private:
	ErrorCode attachDetachStoryObserver(UIDRef storyUIDRef, bool16 bAttach);
	void documentTraversal(UIDRef docRef ,bool16 val);
	json::Object GetCoordObject(PMRect r);
	json::Object charParStyle(ITextModel* textModel,ofstream& textDataFile);
	void getParaStyleInformation(IAttributeStrand* xyz,IWorkspace* theWS,int32 start,int32 count,json::Array & paraStyleArray);
	void getCaraStyleInformation(IAttributeStrand* icastrand,IWorkspace* theWS,int32 start,int32 count,json::Array & caraStyleArray);
	void getTextFrameData(ITextModel* textModel,ofstream& textDataFile);
	json::Object getSpreadDimension(PMRect node);
	void getTextFrameDimension(IMultiColumnTextFrame* MCF,json::Object & frameObject,PMMatrix pb2page);
	void generateTextContentFileGetStyle(IMultiColumnTextFrame* MCF,json::Object & frameObject,int32 spreadIndex,int32 pageIndex,int32 frameIndex);


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
			//DocWchUtils::DwAlert(doc, kAfterNewDocSignalStringKey);
			//if(storyFlag==1)
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
		}*/

	case kBeforeSaveAsDocSignalResponderService:
		{
			InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
			UIDRef doc = docData->GetDocument();		
			//DocWchUtils::DwAlert(doc, kBeforeSaveAsDocSignalStringKey);
			//CAlert::InformationAlert("--------------------111------------------");
			CAlert::InformationAlert("kBeforeSaveAsDocSignalResponderService");
			this->documentTraversal(doc,kTrue);

			break;
		}

		/*
		case kAfterSaveAsDocSignalResponderService:
		{
		InterfacePtr<IDocumentSignalData> docData(signalMgr, UseDefaultIID());
		UIDRef doc = docData->GetDocument();		
		DocWchUtils::DwAlert(doc, kAfterSaveAsDocSignalStringKey);
		break;
		}
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
		InterfacePtr<IObserver> iMyActiveSelectionObserver(doc,IID_IMYACTIVESELECTIONOBSERVER);
		iMyActiveSelectionObserver->AutoAttach(); 
		status = kSuccess;
	}while (kFalse);
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


void DocWchResponder::documentTraversal(UIDRef docRef ,bool16 bAttach) //doc traverse + call to chara para style 
{
	ofstream outfile;
	outfile.open("D:\\DocWch\\GeneratedFiles\\json.txt");
	int graphicFrameCount=0, textFrameCount=0;
	do
	{
		if(!docRef)	           {CAlert::InformationAlert("docRef failed");		break;  }

		InterfacePtr<IDocument> document(docRef, UseDefaultIID());			
		if(!document)	{CAlert::InformationAlert("document failed");	break;	}

		PMString docName;
		document->GetName(docName);

		std::string documentName= docName.GetPlatformString();       
		json::Object docObject;
		docObject["DocumentName"] = documentName; 

		IDataBase* database = ::GetDataBase(document);						
		if(!database)   {CAlert::InformationAlert("database failed");	break;	}

		InterfacePtr<ISpreadList> spreadList(document, UseDefaultIID());    
		if(!spreadList) {CAlert::InformationAlert("spreadList failed");	break;	}

		UIDList frameList(database);
		json::Array spreadListArray;

		int32 spreadCount = spreadList->GetSpreadCount();  
		int32 spreadIndex;
		for (spreadIndex = 0; spreadIndex < spreadCount; spreadIndex++ )
		{
			json::Object spreadObject;

			PMString spreadIndexNum;
			spreadIndexNum.AppendNumber(spreadIndex);
			std::string sprInd= spreadIndexNum.GetPlatformString();       			
			spreadObject["SpreadIndex"]=sprInd;

			//PMMatrix page2pb = ::InnerToPasteboardMatrix(pageGeometry);
			//PMMatrix pb2page = page2pb;
			//pb2page.Invert ();

			InterfacePtr<IGeometry> iSpreadGeometry(spreadList->QueryNthSpread(spreadIndex));
			if(!iSpreadGeometry) {CAlert::InformationAlert("iSpreadGeometry failed.");	break;	}
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
			pb2page3.Invert ();
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
			if(!spread)			{CAlert::InformationAlert("spread failed");		break;	}


			int32 numberOfPages = spread->GetNumPages();
			PMString noPages;
			noPages.AppendNumber(numberOfPages);

			json::Array pageListArray;
			int32 pageIndex;
			for (pageIndex = 0; pageIndex < numberOfPages; pageIndex++ )                                       
			{
				json::Object pageObject;
				json::Array pageCoordArray;

				IGeometry * pageGeometry = spread->QueryNthPage(pageIndex);
				if(!pageGeometry) {CAlert::InformationAlert("pageGeometry failed.");	break;	}

				const PMRect r1 = pageGeometry->GetStrokeBoundingBox();

				json::Object pageCoordObject;	
				pageCoordObject=this->GetCoordObject(r1);						
				pageCoordArray.push_back(pageCoordObject);

				if(pageIndex==0)
				{
					PMMatrix page2pbLocal = ::InnerToPasteboardMatrix(pageGeometry);   //InnerToPasteboardMatrix
					PMMatrix pb2pageLocal = page2pbLocal ;
					pb2pageLocal.Invert ();
					spreadPMRect = iSpreadGeometry->GetPathBoundingBox(pb2pageLocal); 
				}

				PMMatrix page2pb = ::InnerToPasteboardMatrix(pageGeometry);   //InnerToPasteboardMatrix
				PMMatrix pb2page = page2pb;
				pb2page.Invert ();

				PMString pageIndexNum;
				pageIndexNum.AppendNumber(pageIndex);

				UIDList pageItemList(database);
				spread->GetItemsOnPage(pageIndex, &pageItemList, kFalse, kFalse  );
				// don't include the page object itself
				// don't include items that lie on the pasteboard because we only want what's on the page?


				int32 frameIndex;
				int32 pageItemListLength = pageItemList.Length();
				json::Array frameListArray;
				for (frameIndex = 0; frameIndex < pageItemListLength; frameIndex++ )
				{
					json::Object frameObject;

					frameObject["FrameIndex:"]=frameIndex;

					UIDRef pageItemRef = pageItemList.GetRef(frameIndex);


					InterfacePtr<IGraphicFrameData> graphicFrameData(pageItemRef, UseDefaultIID());
					if(graphicFrameData)		{	graphicFrameCount++;}

					//InterfacePtr<IHierarchy> graphicFrameHierarchy(graphicFrameData, UseDefaultIID());
					//if(!graphicFrameHierarchy)	{	CAlert::InformationAlert("graphicFrameHierarchy failed.");	break;}
					//InterfacePtr<IGeometry> fGeometry(graphicFrameHierarchy, UseDefaultIID());
					//if(!fGeometry)	{	CAlert::InformationAlert("fGeometry failed.");	break;}


					UID fromMultiColumnItemUID = graphicFrameData->GetTextContentUID();
					InterfacePtr<IMultiColumnTextFrame> MCF(database, fromMultiColumnItemUID, UseDefaultIID());
					if (MCF == nil)			{	CAlert::InformationAlert("MCF failed.");			}//break;s

					if(MCF!=nil)
					{
						PMString fi;
						fi.AppendNumber(frameIndex);
						//std::string frameIndexStr= fi.GetPlatformString();

						PMString pi;
						pi.AppendNumber(pageIndex);
//						std::string pageIndexStr= fi.GetPlatformString();

						PMString si;
						si.AppendNumber(spreadIndex);
	//					std::string spreadIndexStr= fi.GetPlatformString();
						CAlert::InformationAlert("All Indexes:"+si+","+pi+","+fi);
						this->getTextFrameDimension(MCF,frameObject,pb2page);
						this->generateTextContentFileGetStyle(MCF,frameObject,spreadIndex,pageIndex,frameIndex);

						/*
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
						//::TransformInnerRectToParent( textFrameGeometry,&textFrameBounds);


						frameCoordObject=this->GetCoordObject(textFrameBounds);						
						frameObject["Frame Coord:"]=frameCoordObject;
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

						InterfacePtr<ITextModel> textModel(MCF->QueryTextModel());
						if(textModel)
						{
						UIDRef textModelRef=::GetUIDRef(textModel);

						PMString fi;
						fi.AppendNumber(frameIndex);
						std::string frameIndexStr= fi.GetPlatformString();

						ofstream textDataFile;
						textDataFile.open("D:\\DocWch\\GeneratedFiles\\textDataFile_"+frameIndexStr+".txt");

						this->getTextFrameData(textModel,textDataFile);
						textDataFile<<endl<< endl;

						json::Object frameStyleObject;

						//frameStyleObject=this->charParStyle(textModel,textDataFile);/////////////////////////////////////////////////
						//frameObject["FrameContentStyle"]=frameStyleObject;//////////////////////////////////////////////////////////////////

						textDataFile.close();
						}
						else
						{
						CAlert::InformationAlert("Hiii ...textModel failed.");
						}
						*/
					}
					else
					{
						InterfacePtr<IGeometry> fGeometry(pageItemRef, UseDefaultIID());
						if(!fGeometry)	{	CAlert::InformationAlert("fGeometry failed");	break;	}


						json::Object frameCoordObject;				

						/*
						PMRect bBoxInner = Utils<Facade::IGeometryFacade>()->GetItemBounds( ::GetUIDRef(fGeometry), Transform::InnerCoordinates(), Geometry::OuterStrokeBounds());
						PMRect bBoxParent = Utils<Facade::IGeometryFacade>()->GetItemBounds( ::GetUIDRef(fGeometry), Transform::ParentCoordinates(), Geometry::OuterStrokeBounds());
						PBPMRect bBoxPasteboard = Utils<Facade::IGeometryFacade>()->GetItemBounds( ::GetUIDRef(fGeometry), Transform::PasteboardCoordinates(), Geometry::OuterStrokeBounds());
						*/
						/*
						PMReal a = pb2page.operator[](0);	PMString aS; aS.AppendNumber(a);
						PMReal b = pb2page.operator[](1);	PMString bS; bS.AppendNumber(b);
						PMReal c = pb2page.operator[](2);	PMString cS; cS.AppendNumber(c);
						PMReal d = pb2page.operator[](3);	PMString dS; dS.AppendNumber(d);
						PMReal e = pb2page.operator[](4);	PMString eS; eS.AppendNumber(e);
						PMReal f = pb2page.operator[](5);	PMString fS; fS.AppendNumber(f);
						CAlert::InformationAlert("[a,b,c,d,e,f]:"+aS+","+bS+","+cS+","+dS+","+eS+","+fS);
						*/
						PMRect r = fGeometry->GetPathBoundingBox(pb2page);  //GetStrokeBoundingBox(pb2page);
						frameCoordObject=this->GetCoordObject(r);	
						frameObject["FrameCoord:"]=frameCoordObject;

						/*
						frameCoordObject=this->GetCoordObject(bBoxInner);
						frameCoordObject=this->GetCoordObject(bBoxParent);
						frameCoordObject=this->GetCoordObject(bBoxPasteboard);


						frameObject["Frame Coord:1"]=frameCoordObject;
						frameObject["Frame Coord:2"]=frameCoordObject;
						frameObject["Frame Coord:3"]=frameCoordObject;

						//frameObject["FrameContentStyle"]=frameStyleObject;
						*/
					}
					frameListArray.push_back(frameObject);
					frameObject.Clear();
				}
				std::string pageIndexStr= pageIndexNum.GetPlatformString();    

				pageObject["PageIndex"]=pageIndexStr;
				pageObject["Frames"]=frameListArray;
				pageObject["PageCoord"]=pageCoordArray;
				pageListArray.push_back(pageObject);

			}

			json::Object spreadCoordObject;	
			spreadCoordObject=this->getSpreadDimension(spreadPMRect);						
			spreadObject["SpreadDim"]=spreadCoordObject;

			spreadObject["Pages"]=pageListArray;
			spreadListArray.push_back(spreadObject);
		} 
		docObject["Spreads"] = spreadListArray;
		std::string serialized_json_string = json::Serialize(docObject);
		outfile<<endl<<serialized_json_string<< endl;
		outfile.close();	


	} while(kFalse);	
}


void DocWchResponder::getTextFrameDimension(IMultiColumnTextFrame* MCF,json::Object& frameObject,PMMatrix pb2page)
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

				frameCoordObject=this->GetCoordObject(textFrameBounds);						
				frameObject["FrameCoord:"]=frameCoordObject;
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

void DocWchResponder::generateTextContentFileGetStyle(IMultiColumnTextFrame* MCF,json::Object& frameObject,int32 spreadIndex,int32 pageIndex,int32 frameIndex)
{
	InterfacePtr<ITextModel> textModel(MCF->QueryTextModel());
	if(textModel)
	{
		UIDRef textModelRef=::GetUIDRef(textModel);

		PMString fi;
		fi.AppendNumber(frameIndex);
		std::string frameIndexStr= fi.GetPlatformString();

		PMString pi;
		pi.AppendNumber(pageIndex);
		std::string pageIndexStr= pi.GetPlatformString();

		PMString si;
		si.AppendNumber(spreadIndex);
		std::string spreadIndexStr= si.GetPlatformString();

		ofstream textDataFile;
		textDataFile.open("D:\\DocWch\\GeneratedFiles\\TextDataFile_"+spreadIndexStr+"_"+pageIndexStr+"_"+frameIndexStr+".txt");

		this->getTextFrameData(textModel,textDataFile);


		json::Object frameStyleObject;

		frameStyleObject=this->charParStyle(textModel,textDataFile);
		frameObject["FrameContentStyle"]=frameStyleObject;

		textDataFile.close();
	}
	else
	{
		CAlert::InformationAlert("Hiii ...textModel failed.");
	}
}



json::Object DocWchResponder::GetCoordObject( PMRect r)
{

	json::Object coordObject;
	UTF32TextChar comma=',';
	UTF32TextChar openBracket='(';
	UTF32TextChar closeBracket=')';


	PMPoint rb=r.RightBottom();			
	PMReal rbx = rb.X();
	PMReal rby = rb.Y();		
	PMString rb1;		
	rb1.AppendW(openBracket);
	rb1.AppendNumber(rbx);  
	rb1.AppendW(comma);
	rb1.AppendNumber(rby);
	rb1.AppendW(closeBracket);


	PMPoint tl=r.LeftTop();
	PMReal tlx = tl.X();
	PMReal tly = tl.Y();												
	PMString tl1,tl2;
	tl1.AppendW(openBracket);
	tl1.AppendNumber(tlx);  
	tl1.AppendW(comma);
	tl1.AppendNumber(tly);
	tl1.AppendW(closeBracket);

	PMPoint rt=r.RightTop();
	PMReal rtx = rt.X();
	PMReal rty = rt.Y();												
	PMString rt1;
	rt1.AppendW(openBracket);
	rt1.AppendNumber(rtx);  
	rt1.AppendW(comma);
	rt1.AppendNumber(rty);
	rt1.AppendW(closeBracket);

	PMPoint lb=r.LeftBottom();
	PMReal lbx = lb.X();
	PMReal lby = lb.Y();												
	PMString lb1;
	lb1.AppendW(openBracket);
	lb1.AppendNumber(lbx);  
	lb1.AppendW(comma);
	lb1.AppendNumber(lby);
	lb1.AppendW(closeBracket);


	std::string tlx1= tl1.GetPlatformString();
	std::string rtx1= rt1.GetPlatformString();
	std::string rbx1= rb1.GetPlatformString();
	std::string lbx1= lb1.GetPlatformString();

	coordObject["TopLeft"]=tlx1;
	coordObject["RightTop"]=rtx1;
	coordObject["RightBottom"]=rbx1;
	coordObject["LeftBottom"]=lbx1;


	return coordObject;
}

json::Object DocWchResponder::charParStyle(ITextModel* textModel,ofstream& textDataFile)
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
		int32 start=0;
		int32 count=0,count2=0; // number of chars from start that have this style

		totalTextModelLength=textModel->TotalLength();

		InterfacePtr<IAttributeStrand> ipastrand((IAttributeStrand*)(textModel->QueryStrand(kParaAttrStrandBoss,IID_IATTRIBUTESTRAND)));
		if (!ipastrand)					{	CAlert::InformationAlert("ipastrand Failed");					break;	}	

		InterfacePtr<IAttributeStrand> icastrand((IAttributeStrand*)(textModel->QueryStrand(kCharAttrStrandBoss,IID_IATTRIBUTESTRAND)));
		if (!icastrand)					{	CAlert::InformationAlert("icastrand Failed");					break;	}	

		IActiveContext* activeContext = GetExecutionContextSession()->GetActiveContext();
		if (!activeContext)				{	CAlert::InformationAlert("activeContext Failed");					break;	}	

		IWorkspace * theWS = activeContext->GetContextWorkspace();

		json::Array paraStyleArray;
		json::Array caraStyleArray;

		this->getParaStyleInformation(ipastrand,theWS,start,count,paraStyleArray);
		this->getCaraStyleInformation(icastrand,theWS,start,count,caraStyleArray);

		frameStyleObject["ParaStyle"]=paraStyleArray;
		frameStyleObject["CaraStyle"]=caraStyleArray;

	}while(kFalse);
	return frameStyleObject;
}

void DocWchResponder::getParaStyleInformation(IAttributeStrand* ipastrand,IWorkspace* theWS,int32 start,int32 count,json::Array & paraStyleArray)
{
	do
	{
		if(totalTextModelLength==start)
		{	
			break;
		}

		json::Object styleInfoObject;
		start=start+count; 
		int32 count2;

		int32 count; 
		UID parastyle = ipastrand->GetStyleUID(start,&count2);
		count=count2;

		IActiveContext* activeContext = GetExecutionContextSession()->GetActiveContext();
		if (!activeContext)		{	CAlert::InformationAlert("activeContext Failed");	break;	}	

		InterfacePtr<IStyleInfo> paraStyleInfo(::GetDataBase(theWS),parastyle,UseDefaultIID());
		if(!paraStyleInfo)		{	CAlert::InformationAlert("paraStyleInfo failed");	break;	}

		PMString startCount,totLen;
		startCount.AppendNumber(start);
		totLen.AppendNumber(totalTextModelLength);

		PMString charCount;
		charCount.AppendNumber(count);

		PMString paraStyleName=paraStyleInfo->GetName();
		PMString fromCharNumbers;
		fromCharNumbers.AppendNumber(start);		
		//CAlert::InformationAlert("paraStyleName is:"+paraStyleName+"&&"+fromCharNumbers+" START: "+startCount+" TotLength:"+totLen);
		int32 toNum=start+count;
		PMString toCharNumbers;
		toCharNumbers.AppendNumber(toNum);

		std::string paraStyleNameString= paraStyleName.GetPlatformString();
		std::string fromString= fromCharNumbers.GetPlatformString();
		std::string toString= toCharNumbers.GetPlatformString();
		std::string charCountStr= charCount.GetPlatformString();

		styleInfoObject["ParaStyleName"]=paraStyleNameString;
		styleInfoObject["StyleStart"]=fromString;
		styleInfoObject["CharCount"]=charCountStr;
		if(count!=0)
			paraStyleArray.push_back(styleInfoObject);
		//textDataFile<<"paraStyleName is:"<<paraStyleNameString<<"\t||applied from char number:"<<fromString<<"\t|| to :"<<toString<<endl;
		this->getParaStyleInformation(ipastrand,theWS,start,count,paraStyleArray);

	}while(kFalse);
}

void DocWchResponder::getCaraStyleInformation(IAttributeStrand* icastrand,IWorkspace* theWS,int32 start,int32 count,json::Array & caraStyleArray)
{
	do
	{
		if(totalTextModelLength==start)
		{
			break;
		}

		json::Object styleInfoObject;
		start=start+count; 
		int32 count2;

		int32 count; 
		UID carastyle = icastrand->GetStyleUID(start,&count2);
		count=count2;

		IActiveContext* activeContext = GetExecutionContextSession()->GetActiveContext();
		if (!activeContext)		{	CAlert::InformationAlert("activeContext Failed");	break;	}	

		InterfacePtr<IStyleInfo> caraStyleInfo(::GetDataBase(theWS),carastyle,UseDefaultIID());
		if(!caraStyleInfo)		{	CAlert::InformationAlert("caraStyleInfo failed");	break;	}

		PMString caraStyleName=caraStyleInfo->GetName();
		PMString fromCharNumbers;
		fromCharNumbers.AppendNumber(start);
		//CAlert::InformationAlert("caraStyleName is:"+caraStyleName+" && "+charNumbers);

		int32 toNum=start+count;
		PMString toCharNumbers;
		toCharNumbers.AppendNumber(toNum);

		PMString charCount;
		charCount.AppendNumber(count);

		std::string caraStyleNameString= caraStyleName.GetPlatformString();
		std::string fromString= fromCharNumbers.GetPlatformString();
		std::string toString= toCharNumbers.GetPlatformString();
		std::string charCountStr= charCount.GetPlatformString();

		styleInfoObject["CharaStyleName"]=caraStyleNameString;
		styleInfoObject["StyleStart"]=fromString;
		styleInfoObject["CharCount"]=charCountStr;

		if(count!=0)
			caraStyleArray.push_back(styleInfoObject);
		//textDataFile<<"caraStyleName is:"<<caraStyleNameString<<"\t||applied from char number:"<<fromString<<"\t|| to :"<<toString<<endl;
		this->getCaraStyleInformation(icastrand,theWS,start,count,caraStyleArray);

	}while(kFalse);
}

void DocWchResponder::getTextFrameData(ITextModel* textModel,ofstream& textDataFile)
{
	int32 totLength;
	WideString result;
	PMString fResult;
	totLength=textModel->TotalLength();
	TextIterator begin(textModel,0);
	TextIterator end(textModel,totLength);                   
	for (TextIterator iter = begin; iter<end; iter++) 
	{
		const UTF32TextChar characterCode = *iter;
		result.Append(characterCode);
	} 
	fResult.Append(result);
	std::string textData= fResult.GetPlatformString();
	textDataFile<<textData;
}

json::Object DocWchResponder::getSpreadDimension(PMRect spreadPMRect)
{
	json::Object sprDim,refer;

	PMReal a = spreadPMRect.Top();		PMString top;		top.AppendNumber(a);
	PMReal b = spreadPMRect.Left();		PMString left;		left.AppendNumber(b);
	PMReal c = spreadPMRect.Bottom();	PMString bottom;	bottom.AppendNumber(c);
	PMReal d = spreadPMRect.Right();	PMString right;		right.AppendNumber(d);

	std::string topStr= top.GetPlatformString();
	std::string leftStr= left.GetPlatformString();
	std::string bottomStr= bottom.GetPlatformString();
	std::string rightStr= right.GetPlatformString();
	refer["Top"]=topStr;
	refer["Left"]=leftStr;
	refer["Bottom"]=bottomStr;
	refer["Right"]=rightStr;


	PMReal spreadHeight=spreadPMRect.Height();
	PMReal spreadWidth=spreadPMRect.Width();
	PMString sprHgt;	sprHgt.AppendNumber(spreadHeight);
	PMString sprWdt;	sprWdt.AppendNumber(spreadWidth);


	std::string sprHgtStr= sprHgt.GetPlatformString();
	std::string sprWdtStr= sprWdt.GetPlatformString();
	sprDim["SpreadHeight"]=sprHgtStr;
	sprDim["SpreadWidth"]=sprWdtStr;
	sprDim["SpreadReference"]=refer;

	return sprDim;
}
