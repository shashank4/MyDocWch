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
	json::Object GetCoordObject(const PMRect r);
	void charParStyle(ITextModel* textModel);
	void getParaStyleInformation(IAttributeStrand* xyz,IWorkspace* theWS,int32 start,int32 count);
	void getCaraStyleInformation(IAttributeStrand* icastrand,IWorkspace* theWS,int32 start,int32 count);
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


void DocWchResponder::documentTraversal(UIDRef docRef ,bool16 bAttach)
{



	/*
	const PMReal tlx;
	const PMReal tly;
	const PMReal rbx;
	const PMReal rby;
	*/
	/*
	PMString data("This is to be written inside a text file"); 
	PMString path("D:\\temp.txt"); 
	IDFile txtFile(path); 
	IPMStream * txtFileOutStream = StreamUtil::CreateFileStreamWriteLazy (txtFile); 
	if(txtFileOutStream == nil)  {CAlert::InformationAlert("txtFileOutStream failed");	}
	*/

	CAlert::InformationAlert("documentTraversal");
	ofstream outfile;
	outfile.open("D:\\DocWch\\temp.txt");

	outfile <<endl<< endl;

	int graphicFrameCount=0, textFrameCount=0;
	do
	{
		if(!docRef)	           {CAlert::InformationAlert("docRef failed");		break;  }

		InterfacePtr<IDocument> document(docRef, UseDefaultIID());			if(!document)	{CAlert::InformationAlert("document failed");	break;	}

		PMString docName;
		document->GetName(docName);

		std::string documentName= docName.GetPlatformString();       
		json::Object docObject;
		docObject["Name"] = documentName; 

		IDataBase* database = ::GetDataBase(document);						if(!database)          {CAlert::InformationAlert("database failed");	break;	}

		InterfacePtr<ISpreadList> spreadList(document, UseDefaultIID());    if(!spreadList)        {CAlert::InformationAlert("spreadList failed");	break;	}

		UIDList frameList(database);
		json::Array spreadListArray;

		int32 spreadCount = spreadList->GetSpreadCount();                                     
		for (int32 spreadIndex = 0; spreadIndex < spreadCount; spreadIndex++ )
		{
			json::Object spreadObject;

			PMString spreadIndexNum;
			spreadIndexNum.AppendNumber(spreadIndex);
			std::string sprInd= spreadIndexNum.GetPlatformString();       			
			spreadObject["Index"]=sprInd;

			UIDRef spreadUIDRef(database, spreadList->GetNthSpreadUID(spreadIndex));

			InterfacePtr<ISpread> spread(spreadUIDRef, UseDefaultIID());	if(!spread)			{CAlert::InformationAlert("spread failed");		break;	}

			int32 numberOfPages = spread->GetNumPages();
			PMString noPages;
			noPages.AppendNumber(numberOfPages);



			json::Array pageListArray;
			for (int32 nPage = 0; nPage < numberOfPages; nPage++ )                                       
			{
				json::Object pageObject;
				json::Array pageCoordArray;

				IGeometry * pageGeometry = spread->QueryNthPage(nPage);
				if(!pageGeometry) {CAlert::InformationAlert("pageGeometry failed.");	break;	}
				const PMRect r1 = pageGeometry->GetStrokeBoundingBox();
				json::Object coordObject;	
				coordObject=this->GetCoordObject(r1);						
				pageCoordArray.push_back(coordObject);



				PMString pageIndexNum;
				pageIndexNum.AppendNumber(nPage);

				std::string pageIndexStr= pageIndexNum.GetPlatformString();                                        

				UIDList pageItemList(database);
				spread->GetItemsOnPage(nPage, &pageItemList, kFalse, kFalse   );
				// don't include the page object itself
				// don't include items that lie on the pasteboard because we only want what's on the page?

				json::Array coordObjectArray;

				int32 pageItemListLength = pageItemList.Length();
				for (int32 j = 0; j < pageItemListLength; j++ )
				{
					UIDRef pageItemRef = pageItemList.GetRef(j);


					InterfacePtr<IGraphicFrameData> graphicFrameData(pageItemRef, UseDefaultIID());
					/*if (graphicFrameData != nil) 
					{
					int32 absolPage = this->GlobalFromLocalPageIndex(spreadList, spreadIndex, nPage);
					this->addGraphicFrameDescription(docRef,
					pageItemRef,
					collectFrames, 
					absolPage);
					}*/
					if(graphicFrameData)	{	graphicFrameCount++;}


					///////////////////////////////////
									
					UID fromMultiColumnItemUID = graphicFrameData->GetTextContentUID();
					InterfacePtr<IMultiColumnTextFrame> MCF(database, fromMultiColumnItemUID, UseDefaultIID());
					if (MCF == nil)			{	CAlert::InformationAlert("MFC failed.");			break;		}
					
					InterfacePtr<ITextModel> textModel(MCF->QueryTextModel());
					
					//UIDRef textModelUIDRef(database, graphicFrameData->GetTextContentUID());
					//InterfacePtr<ITextModel> textModel(graphicFrameData->GetTextContentUID(), UseDefaultIID());
					
					if(textModel)
					{
						InterfacePtr<IFrameList> frameList(textModel->QueryFrameList());

						int32 frameCount = frameList->GetFrameCount();
						textFrameCount=textFrameCount+frameCount;
						CAlert::InformationAlert("Hiii ...textModel created.");
						this->charParStyle(textModel);
					}
					else
					{
						CAlert::InformationAlert("Hiii ...textModel failed.");
					}

					InterfacePtr<IGeometry> fGeometry(pageItemRef, UseDefaultIID());
					

					if(!fGeometry)	{	CAlert::InformationAlert("fGeometry failed");	break;	}

					json::Object coordObject;				


					PMMatrix page2pb = ::InnerToPasteboardMatrix(pageGeometry);
					PMMatrix pb2page = page2pb;
					pb2page.Invert ();

					const PMRect r = fGeometry->GetPathBoundingBox(pb2page);  //GetStrokeBoundingBox(pb2page);

					coordObject=this->GetCoordObject(r);						
					coordObjectArray.push_back(coordObject);

				}

				pageObject["Index"]=pageIndexStr;
				pageObject["Frames"]=coordObjectArray;
				pageObject["Page_Co-ord"]=pageCoordArray;
				pageListArray.push_back(pageObject);

			}
			spreadObject["Pages"]=pageListArray;
			spreadListArray.push_back(spreadObject);
		} 
		docObject["Spreads"] = spreadListArray;
		std::string serialized_json_string = json::Serialize(docObject);
		outfile<<endl<<serialized_json_string<< endl;
		outfile.close();	

	} while(kFalse);	
}


json::Object DocWchResponder::GetCoordObject(const PMRect r)
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



void DocWchResponder::charParStyle(ITextModel* textModel)
{
	CAlert::InformationAlert("--------------------111111111------------------");
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


		this->getParaStyleInformation(ipastrand,theWS,start,count);/////////////////////////////////
		this->getCaraStyleInformation(icastrand,theWS,start,count);



		//InterfacePtr<IStyleInfo> caraStyleInfo(::GetDataBase(theWS),carastyle,UseDefaultIID());
		//if(!caraStyleInfo)		{	CAlert::InformationAlert("caraStyleInfo failed");	break;	}
		//
		//PMString caraStyleName=caraStyleInfo->GetName();
		//PMString charNumbers,charNumbers2;
		//charNumbers.AppendNumber(count);
		//charNumbers2.AppendNumber(count2);
		//CAlert::InformationAlert("***caraStyleName is:"+caraStyleName+"\\\\\\"+charNumbers2);


	}while(kFalse);


}


void DocWchResponder::getParaStyleInformation(IAttributeStrand* ipastrand,IWorkspace* theWS,int32 start,int32 count)
{

	do
	{
		/*
		// isParaStyle = styleInfo->GetStyleType() == IStyleInfo::kParagraphStyle;

		InterfacePtr<IStyleInfo> paraStyleInfo(::GetDataBase(theWS),parastyle,UseDefaultIID());
		if(!paraStyleInfo)		{	CAlert::InformationAlert("paraStyleInfo failed");	break;	}


		PMString paraStyleName=paraStyleInfo->GetName();
		PMString charNumbers;
		charNumbers.AppendNumber(count);
		CAlert::InformationAlert("paraStyleName is:"+paraStyleName+"/////"+charNumbers);
		UID nextParaStyle=paraStyleInfo->GetNextStyle();
		if(UID!=NULL)
		{
		this->getParaStyleInformation(theWS,nextParaStyle);
		}

		*/

		CAlert::InformationAlert("----------------getParaStyleInformation----------------------");

		if(totalTextModelLength==start)
		{
			CAlert::InformationAlert("Reached to end.");
			break;
		}
		start=start+count; 
		int32 count2;

		int32 count; 
		UID parastyle = ipastrand->GetStyleUID(start,&count2);
		count=count2;

		IActiveContext* activeContext = GetExecutionContextSession()->GetActiveContext();
		if (!activeContext)				{	CAlert::InformationAlert("activeContext Failed");					break;	}	

		InterfacePtr<IStyleInfo> paraStyleInfo(::GetDataBase(theWS),parastyle,UseDefaultIID());
		if(!paraStyleInfo)		{	CAlert::InformationAlert("paraStyleInfo failed");	break;	}

		PMString startCount,totLen;
		startCount.AppendNumber(start);
		totLen.AppendNumber(totalTextModelLength);
		
		
		PMString paraStyleName=paraStyleInfo->GetName();
		PMString charNumbers;
		charNumbers.AppendNumber(count);		
		CAlert::InformationAlert("paraStyleName is:"+paraStyleName+"&&"+charNumbers+" START: "+startCount+" TotLength:"+totLen);

		this->getParaStyleInformation(ipastrand,theWS,start,count);



	}while(kFalse);
}

void DocWchResponder::getCaraStyleInformation(IAttributeStrand* icastrand,IWorkspace* theWS,int32 start,int32 count)
{

	do
	{
		/*
		// isParaStyle = styleInfo->GetStyleType() == IStyleInfo::kParagraphStyle;

		InterfacePtr<IStyleInfo> paraStyleInfo(::GetDataBase(theWS),parastyle,UseDefaultIID());
		if(!paraStyleInfo)		{	CAlert::InformationAlert("paraStyleInfo failed");	break;	}


		PMString paraStyleName=paraStyleInfo->GetName();
		PMString charNumbers;
		charNumbers.AppendNumber(count);
		CAlert::InformationAlert("paraStyleName is:"+paraStyleName+"/////"+charNumbers);
		UID nextParaStyle=paraStyleInfo->GetNextStyle();
		if(UID!=NULL)
		{
		this->getParaStyleInformation(theWS,nextParaStyle);
		}

		*/

		CAlert::InformationAlert("----------------getCaraStyleInformation----------------------");

		if(totalTextModelLength==start)
		{
			CAlert::InformationAlert("Reached to end.");
			break;
		}
		start=start+count; 
		int32 count2;

		int32 count; 
		UID carastyle = icastrand->GetStyleUID(start,&count2);
		count=count2;

		IActiveContext* activeContext = GetExecutionContextSession()->GetActiveContext();
		if (!activeContext)				{	CAlert::InformationAlert("activeContext Failed");					break;	}	

		InterfacePtr<IStyleInfo> caraStyleInfo(::GetDataBase(theWS),carastyle,UseDefaultIID());
		if(!caraStyleInfo)		{	CAlert::InformationAlert("caraStyleInfo failed");	break;	}
		
		

		PMString caraStyleName=caraStyleInfo->GetName();
		PMString charNumbers;
		charNumbers.AppendNumber(count);
		CAlert::InformationAlert("caraStyleName is:"+caraStyleName+" && "+charNumbers);

		this->getCaraStyleInformation(icastrand,theWS,start,count);



	}while(kFalse);
}