
#include "VCPlugInHeaders.h"

// Interface includes:
#include "ISubject.h"
#include "IDocument.h"
#include "IObjectModel.h"
#include "ITextFocus.h"
#include "ITextFocusManager.h"
#include "ITextModel.h"            
#include "IStoryList.h"
#include "IObserver.h"
#include "ITextTarget.h"
#include "IActiveContext.h"
#include "ISelectionUtils.h"
#include "IIntegratorTarget.h"
#include "ITextSelectionSuite.h"
#include "ILayoutSelectionSuite.h"
#include "ILayoutTarget.h"
#include "IGraphicAttributeInfo.h"
#include "IGraphicFrameData.h"
#include "IXMLReferenceData.h"
#include "IIDXMLElement.h"
#include "IHierarchy.h"
#include "IFrameList.h"

#include "ITableSelectionSuite.h"
#include "ITableTarget.h"
#include "ITableModel.h"
#include "ITableSuite.h"
#include "ITextStoryThreadDict.h"
#include "ITextStoryThread.h"

#include "IDrawingStyle.h"
#include "IComposeScanner.h"
#include "IStyleGroupManager.h"
#include "IStyleInfo.h"
#include "IStyleNameTable.h"
#include "IWorkspace.h"

#include "IAttributeStrand.h"

#include "ISnippetExport.h"
#include "IScript.h"
#include "IScriptUtils.h"
#include "IDOMElement.h"
#include "IK2ServiceRegistry.h"
#include "ISelectionUtils.h"
#include "IExportProvider.h"
#include "IK2ServiceProvider.h"
#include "ILayoutUIUtils.h"


// General includes:
#include "TextEditorID.h"
#include "TextID.h"
#include "CObserver.h"
#include "Utils.h"
#include "UIDList.h"
#include "CAlert.h"
#include "SelectionObserver.h"        
#include "DocFrameworkID.h"
#include "textiterator.h"
#include "TextRange.h"
#include "CmdUtils.h"
#include "PMString.h"
#include "XMLReference.h"
#include "UIDRef.h"

#include "FileUtils.h"
#include "IDFile.h"
#include "StreamUtil.h"
#include "ScriptingDefs.h"
#include "AppPrefsListElementTypes.h"
#include "TransformUtils.h"

/*
#include "TextEditorID.h"	// kIBeamToolBoss
#include "PageItemScrapID.h" // kDeleteCmdBoss
#include "GenericID.h" // kConvertItemToTextCmdBoss
#include "PersistUtils.h"
#include "SDKLayoutHelper.h"
*/

namespace InDesign { class TextRange;}

//Project includes
#include "DocWchID.h"
#include "DocWchUtils.h"

//#include "SnipRunLog.h"

using namespace std;


class MyActiveSelectionObserver: public ActiveSelectionObserver
{
public:
	MyActiveSelectionObserver(IPMUnknown* boss);
	~MyActiveSelectionObserver() {};
	virtual void AutoAttach();
	virtual void AutoDetach();

protected:
	virtual void HandleSelectionChanged(const ISelectionMessage* selectionMessage);

private:
	void onSelectionChange(const ISelectionMessage* selectionMessage);
	void InspectLayoutObject(const UIDRef& pageItemRef);
	UIDRef GetTextModelRef(const InterfacePtr<IGraphicFrameData>& graphicFrameData);
	void TagTraverse(XMLReference xmlRef);
	void layoutSelection();
	void textSelection();
	void tableSelection();
	//void cellTextSelection(ITableSelectionSuite* iTableSelectionSuite );
	void getText(ITextStoryThread* textStoryThread);
	void documentTraversal(UIDRef docRef ,bool16 bAttach);
	void styleCharaPara();
	void exportSelectionHTML(IGraphicFrameData* xyz);
};

CREATE_PMINTERFACE(MyActiveSelectionObserver, kMyActiveSelectionObserverImpl)

	MyActiveSelectionObserver::MyActiveSelectionObserver(IPMUnknown *boss):ActiveSelectionObserver(boss, IID_IMYACTIVESELECTIONOBSERVER){}


void MyActiveSelectionObserver::AutoAttach()
{
	ActiveSelectionObserver::AutoAttach();
} 

void MyActiveSelectionObserver::AutoDetach()
{
	CAlert::InformationAlert("In the AutoDetach");
	ActiveSelectionObserver::AutoDetach();
} 

void MyActiveSelectionObserver::HandleSelectionChanged(const ISelectionMessage* selectionMessage)
{
	this->onSelectionChange(selectionMessage);
}

void MyActiveSelectionObserver::onSelectionChange(const ISelectionMessage* selectionMessage)
{
	//this->textSelection();
	this->layoutSelection();
	//this->tableSelection();

	//this->styleCharaPara();
	//this->exportSelectionHTML();
}

UIDRef MyActiveSelectionObserver::GetTextModelRef(const InterfacePtr<IGraphicFrameData>& graphicFrameData)    //TextModel from Graphic_Frame_Data 
{
	UIDRef result = UIDRef::gNull;
	do {
		if (!graphicFrameData) {
			break;
		}
		// IGraphicFrameData references any associated kMultiColumnItemBoss object.
		UID textframeUID = graphicFrameData->GetTextContentUID();
		if (textframeUID == kInvalidUID) {
			break;
		}

		InterfacePtr<IMultiColumnTextFrame> mcf(::GetDataBase(graphicFrameData), textframeUID, UseDefaultIID());
		ASSERT(mcf);
		if(!mcf) {
			break;
		}
		UID textModelUID = mcf->GetTextModelUID();
		ASSERT(textModelUID != kInvalidUID);
		if (textModelUID == kInvalidUID) {
			break;
		}
		result = UIDRef(::GetDataBase(graphicFrameData), textModelUID);
	} while(false);
	return result;
}

void MyActiveSelectionObserver::textSelection() ////////////////////////Frame and Text Selection......
{
	WideString result,item;
	UIDRef layoutRef;

	do
	{
		InterfacePtr<ITextSelectionSuite> iTextSelectionSuite(fCurrentSelection, UseDefaultIID());
		if (!iTextSelectionSuite)
		{
			break;
		}
		else
		{
			InterfacePtr<const IIntegratorTarget> iIntegratorTarget_text(iTextSelectionSuite, UseDefaultIID());
			if(iIntegratorTarget_text == nil)
				return;

			RangeData rangeData(0,RangeData::kLeanForward);
			UIDRef textRef;

			std::auto_ptr<IIntegratorTarget::TargetSuiteCollection> selectionSuites_text(iIntegratorTarget_text->GetTarget(ITextTarget::kDefaultIID));
			for(IIntegratorTarget::TargetSuiteCollection::size_type i = 0; i < selectionSuites_text->size(); i++)
			{
				ITextTarget* target = (ITextTarget*)selectionSuites_text->at(i).get();
				if(!target)
					continue;
				rangeData = target->GetRange();
				textRef = target->GetTextModel();
				if(textRef == nil)
					return;
				else
				{
					WideString result;
					do
					{
						InterfacePtr<ITextModel> textModel(textRef, UseDefaultIID());
						TextIndex startIndex=rangeData.Start(nil);

						InterfacePtr<IFrameList> frameList(textModel->QueryFrameList());
						int32 frameCount = frameList->GetFrameCount();
						int32 frameIndex = frameList->GetFrameIndexContaining(startIndex);
						PMString IDframe;
						IDframe.AppendNumber(frameIndex);
						CAlert::InformationAlert("frameID: "+IDframe);

						/*           //frame ID...
						for(int32 i=0; i<frameCount;i++)
						{
						UID frameID=frameList->GetNthFrameUID(i);
						PMString IDframe;
						IDframe.AppendNumber(frameID);
						CAlert::InformationAlert("frameID: "+IDframe);
						}
						*/
						/*TextIterator begin(textModel,rangeData.Start(nil));                           // selected text
						TextIterator end(textModel,rangeData.End());                   
						for (TextIterator iter = begin; iter<end; iter++) 
						{
						const UTF32TextChar characterCode = *iter;
						result.Append(characterCode);
						} 

						CAlert::InformationAlert("You Selected : "+result);	*/
					} while(false);	
				} 
			}
		}
	}while (kFalse);
}

void MyActiveSelectionObserver::layoutSelection()///////////////////////////////          layout and Tags
{
	WideString result,item;
	UIDRef layoutRef;

	do
	{
		InterfacePtr<ILayoutSelectionSuite> iLayoutSelectionSuite(fCurrentSelection, UseDefaultIID());
		if (iLayoutSelectionSuite == nil) 
		{
			break;
		}


		InterfacePtr<const IIntegratorTarget> iIntegratorTarget_layout(iLayoutSelectionSuite, UseDefaultIID());
		if(iIntegratorTarget_layout == nil)
		{
			CAlert::InformationAlert("iIntegratorTarget_layout failed");
			break;
		}

		std::auto_ptr<IIntegratorTarget::TargetSuiteCollection> selectionSuites_layout(iIntegratorTarget_layout->GetTarget(ILayoutTarget::kDefaultIID));

		for(IIntegratorTarget::TargetSuiteCollection::size_type i = 0; i < selectionSuites_layout->size(); i++)
		{
			ILayoutTarget* iLayoutTarget = (ILayoutTarget*)selectionSuites_layout->at(i).get();
			if(!iLayoutTarget)
				continue;


			const UIDList	selectedItems(iLayoutTarget->GetUIDList(kStripStandoffs));

			
			int32 pageItemListLength = selectedItems.Length();


			for (int32 j = 0; j < pageItemListLength; j++ )
			{
				UIDRef pageItemRef = selectedItems.GetRef(j);
				InterfacePtr<IGraphicFrameData> graphicFrameData(pageItemRef, UseDefaultIID());
				if (graphicFrameData == nil)
				{break;}

				this->exportSelectionHTML(graphicFrameData);

				//if (graphicFrameData->IsGraphicFrame() == kTrue)
				//;{
				/*
				InterfacePtr<IGraphicAttributeInfo> iGraphicAttributeInfo(pageItemRef, UseDefaultIID());
				if(iGraphicAttributeInfo==nil)
				{
				CAlert::InformationAlert("******************444444*************");
				break;
				}
				PMString* abc=iGraphicAttributeInfo->GetName();
				*/


				UIDRef textModelRef=this->GetTextModelRef(graphicFrameData);
				InterfacePtr<ITextModel> textModel(textModelRef, UseDefaultIID());
				if(textModel)
				{
					CAlert::InformationAlert("iTextModel failed");
				}


				/*Remove this if needed*/			this->InspectLayoutObject(pageItemRef);
				//	CAlert::InformationAlert("Frame is selected:");
				//}	
				/*
				/////////////////**************************************
				SDKLayoutHelper layoutHelper;
				InterfacePtr<ITextModel> textModel(layoutHelper.GetTextModelRef(graphicFrameData), UseDefaultIID());
				if (!textModel) 
				{
				break;
				}

				// Ask which TextIndex	 should be selected.
				Utils<ISnipRunParameterUtils> parameterUtils;
				int32 totalLength = textModel->TotalLength();
				TextIndex at = 0;
				if (totalLength > 1) 
				{
				at = parameterUtils->GetInt32("TextIndex", 0, 0, totalLength-1);
				if (parameterUtils->WasDialogCancelled()) 
				{
				break;
				}	
				}

				InterfacePtr<ITextFrameColumn> textFrameColumn(this->QueryTextFrameContaining(textModel, at));
				if (!textFrameColumn) 
				{
				break;
				}
				UIDRef graphicFrameRef = this->GetGraphicFrameRef(textFrameColumn, kFalse);*/
				/*if (graphicFrameRef == UIDRef::gNull) 
				{
				break;
				}
				*/
				// Select that frame.
				//if (selectionManager->SelectionExists (kInvalidClass/*any CSB*/, ISelectionManager::kAnySelection)) 
				//{
				// Clear the selection
				//	selectionManager->DeselectAll(nil);
				//}
				//layoutSelectionSuite->SelectPageItems(UIDList(graphicFrameRef), Selection::kReplace, Selection::kAlwaysCenterInView);
				//				}				
			}
			//}
			//}
			/*
			InterfacePtr<ITextSelectionSuite> iTextSelectionSuite(fCurrentSelection, UseDefaultIID());
			if (iTextSelectionSuite == nil) 
			{
			CAlert::InformationAlert("iTextSelectionSuite Failed");
			}
			else
			{
			InterfacePtr<const IIntegratorTarget> iIntegratorTarget_text(iTextSelectionSuite, UseDefaultIID());
			if(iIntegratorTarget_text == nil)
			break;

			InterfacePtr<const IIntegratorTarget> iIntegratorTarget_text(iTextSelectionSuite, UseDefaultIID());
			if(iIntegratorTarget_text == nil)
			break;

			RangeData rangeData(0,RangeData::kLeanForward);
			UIDRef textRef;

			std::auto_ptr<IIntegratorTarget::TargetSuiteCollection> selectionSuites_text(iIntegratorTarget_text->GetTarget(ITextTarget::kDefaultIID));
			for(IIntegratorTarget::TargetSuiteCollection::size_type i = 0; i < selectionSuites_text->size(); i++)
			{
			ITextTarget* target = (ITextTarget*)selectionSuites_text->at(i).get();
			if(!target)
			continue;

			rangeData = target->GetRange();	
			textRef = target->GetTextModel();

			if(textRef == nil)
			break;

			InterfacePtr<ITextModel> textModel(textRef,UseDefaultIID());

			TextIterator begin(textModel,rangeData.Start(nil));
			TextIterator end(textModel,rangeData.End());


			for (TextIterator iter = begin; iter != end; iter++) 
			{
			const UTF32TextChar characterCode = *iter;
			result.Append(characterCode);
			}

			CAlert::InformationAlert("Selected text is:"+result);
			}*/
		}
	}while(kFalse);
}




void MyActiveSelectionObserver::InspectLayoutObject(const UIDRef& pageItemRef)
{
	CAlert::InformationAlert("In the InspectLayoutObject fun...");
	do 
	{
		InterfacePtr<IGraphicFrameData> graphicFrame(pageItemRef, UseDefaultIID());
		if(!graphicFrame) 
		{
			CAlert::InformationAlert("graphicFrame failed");
			break;
		}

		UIDRef textModelRef=this->GetTextModelRef(graphicFrame);
		InterfacePtr<ITextModel> textModel(textModelRef, UseDefaultIID());
		if(!textModel)
		{
			CAlert::InformationAlert("iTextModel failed");
		}

		/*RangeData rangeData(0,RangeData::kLeanForward);
		InterfacePtr<ITextTarget> theTarget(this, UseDefaultIID());
		rangeData = theTarget->GetRange();
		TextIndex startIndex = rangeData.Start(nil);
		*/
		//PMString startInd;
		//startInd.AppendNumber(startIndex);
		//CAlert::InformationAlert("Cursor is at :"+startInd);

		/*
		InterfacePtr<IHierarchy> graphicFrameHierarchy(graphicFrame, UseDefaultIID());
		if(!graphicFrameHierarchy) 
		{
		CAlert::InformationAlert("graphicFrameHierarchy failed");
		break;
		}

		InterfacePtr<IHierarchy> childHierarchy(graphicFrameHierarchy->QueryChild(0));
		if(!childHierarchy) 
		{
		CAlert::InformationAlert("childHierarchy failed....");
		break;
		}
		*/

		InterfacePtr<IXMLReferenceData> xmlReferenceData(textModel, UseDefaultIID());
		if(!xmlReferenceData) 
		{
			CAlert::InformationAlert("xmlReferenceData failed");	
			break;
		}

		XMLReference xmlRef = xmlReferenceData->GetReference();

		this->TagTraverse(xmlRef);

	} while(kFalse);
}

void MyActiveSelectionObserver::TagTraverse(XMLReference xmlRef)
{
	InterfacePtr<IIDXMLElement> element(xmlRef.Instantiate());
	if(element)		
	{
		PMString tagName=element->GetTagString();
		CAlert::InformationAlert("Tag name is :"+tagName);

		int32 count=element->GetChildCount();

		PMString number1;
		number1.AppendNumber(count);
		CAlert::InformationAlert("Inside \""+tagName+"\" tags are : "+number1);
		if(count!=0)
		{
			for(int32 i=0;i<count;i++)
			{
				XMLReference childXmlRef = element->GetNthChild(i);
				this->TagTraverse(childXmlRef);
			}
		}	
	} 
}

void MyActiveSelectionObserver::tableSelection()
{
	int row=2;
	int col=2;
	do
	{
		InterfacePtr<ITableSelectionSuite> iTableSelectionSuite(fCurrentSelection, UseDefaultIID());
		if (!iTableSelectionSuite)
		{
			break;
		}
		else
		{
			InterfacePtr<const IIntegratorTarget> iIntegratorTarget_table(iTableSelectionSuite, UseDefaultIID());
			if(iIntegratorTarget_table == nil)
				return;

			RangeData rangeData(0,RangeData::kLeanForward);
			UIDRef tableRef;

			std::auto_ptr<IIntegratorTarget::TargetSuiteCollection> selectionSuites_table(iIntegratorTarget_table->GetTarget(ITableTarget::kDefaultIID));
			for(IIntegratorTarget::TargetSuiteCollection::size_type i = 0; i < selectionSuites_table->size(); i++)
			{
				ITableTarget* tableTarget = (ITableTarget*)selectionSuites_table->at(i).get();
				if(!tableTarget)
					continue;

				InterfacePtr<ITableModel> tableModel(tableTarget->QueryModel());
				if(!tableModel)
				{
					CAlert::InformationAlert("tableModel failed");
					break;
				}

				//tableRef = tableTarget->GetModel();
				bool16 hasContent=tableTarget->HasContent();
				//CAlert::InformationAlert("Table is selected.");



				if(hasContent==kTrue)
				{
					//CAlert::InformationAlert("Has Content.");
					//GridArea selectedText=iTableSelectionSuite->GetSelection();

					//InterfacePtr<ITableSuite> tableSuite(fCurrentSelection, UseDefaultIID());
					/*    Number of columns visible to user.
					int32 userNumCols=tableSuite->GetUserNumColsInTable();
					PMString UNC("No.of cols :");
					UNC.AppendNumber(userNumCols);
					CAlert::InformationAlert(UNC);
					*/
					/*        //text cell selection  (click+ esc)
					bool16 textCellSelected=tableSuite->IsAnyTextCellSelected();
					if(textCellSelected==kTrue)
					{
					CAlert::InformationAlert("Text Cell is selected.");
					}
					*/


					//this->cellTextSelection(iTableSelectionSuite);

					//GridAddress gridAdd= iTableSelectionSuite->GetAnchor();
					GridAddress gridAdd(row, col);

					GridID targetCellGridID = tableModel->GetGridID(gridAdd);
					if(!targetCellGridID)
					{
						CAlert::InformationAlert("targetCellGridID failed");
						break;
					}

					InterfacePtr<ITextStoryThreadDict> textStoryThreadDict(tableModel, UseDefaultIID());
					if (textStoryThreadDict == nil) 
					{
						CAlert::InformationAlert("textStoryThreadDict failed");
						break;
					}

					InterfacePtr<ITextStoryThread> textStoryThread(textStoryThreadDict->QueryThread(targetCellGridID));
					if (textStoryThread == nil)
					{
						CAlert::InformationAlert("textStoryThread failed");
						break;
					}

					this->getText(textStoryThread);
				}

				/*Remove this if needed*/			//this->InspectTableObject(pageItemRef);
				//	CAlert::InformationAlert("Frame is selected:");
				//}	
				/*
				/////////////////**************************************
				SDKLayoutHelper layoutHelper;
				InterfacePtr<ITextModel> textModel(layoutHelper.GetTextModelRef(graphicFrameData), UseDefaultIID());
				if (!textModel) 
				{
				break;
				}

				// Ask which TextIndex	 should be selected.
				Utils<ISnipRunParameterUtils> parameterUtils;
				int32 totalLength = textModel->TotalLength();
				TextIndex at = 0;
				if (totalLength > 1) 
				{
				at = parameterUtils->GetInt32("TextIndex", 0, 0, totalLength-1);
				if (parameterUtils->WasDialogCancelled()) 
				{
				break;
				}	
				}

				InterfacePtr<ITextFrameColumn> textFrameColumn(this->QueryTextFrameContaining(textModel, at));
				if (!textFrameColumn) 
				{
				break;
				}
				UIDRef graphicFrameRef = this->GetGraphicFrameRef(textFrameColumn, kFalse);*/
				/*if (graphicFrameRef == UIDRef::gNull) 
				{
				break;
				}
				*/
				/*		// Select that frame.
				//if (selectionManager->SelectionExists (kInvalidClass, ISelectionManager::kAnySelection)) 
				//{
				// Clear the selection
				//	selectionManager->DeselectAll(nil);
				//}
				//layoutSelectionSuite->SelectPageItems(UIDList(graphicFrameRef), Selection::kReplace, Selection::kAlwaysCenterInView);
				//				}				
				//}
				//}
				//}*/
				/*
				InterfacePtr<ITextSelectionSuite> iTextSelectionSuite(fCurrentSelection, UseDefaultIID());
				if (iTextSelectionSuite == nil) 
				{
				CAlert::InformationAlert("iTextSelectionSuite Failed");
				}
				else
				{
				InterfacePtr<const IIntegratorTarget> iIntegratorTarget_text(iTextSelectionSuite, UseDefaultIID());
				if(iIntegratorTarget_text == nil)
				break;

				InterfacePtr<const IIntegratorTarget> iIntegratorTarget_text(iTextSelectionSuite, UseDefaultIID());
				if(iIntegratorTarget_text == nil)
				break;

				RangeData rangeData(0,RangeData::kLeanForward);
				UIDRef textRef;

				std::auto_ptr<IIntegratorTarget::TargetSuiteCollection> selectionSuites_text(iIntegratorTarget_text->GetTarget(ITextTarget::kDefaultIID));
				for(IIntegratorTarget::TargetSuiteCollection::size_type i = 0; i < selectionSuites_text->size(); i++)
				{
				ITextTarget* target = (ITextTarget*)selectionSuites_text->at(i).get();
				if(!target)
				continue;

				rangeData = target->GetRange();	
				textRef = target->GetTextModel();

				if(textRef == nil)
				break;

				InterfacePtr<ITextModel> textModel(textRef,UseDefaultIID());

				TextIterator begin(textModel,rangeData.Start(nil));
				TextIterator end(textModel,rangeData.End());


				for (TextIterator iter = begin; iter != end; iter++) 
				{
				const UTF32TextChar characterCode = *iter;
				result.Append(characterCode);
				}

				CAlert::InformationAlert("Selected text is:"+result);
				}*/
			}
		}
	}while (kFalse);
}

void MyActiveSelectionObserver::getText(ITextStoryThread* textStoryThread)
{
	WideString text;
	do
	{
		InterfacePtr<ITextModel> textModel(textStoryThread->QueryTextModel());
		if (textModel == nil) {
			break;
		}

		int32 span=(-1);
		TextIndex at = textStoryThread->GetTextStart(&span);
		TextIterator beginTextChunk(textModel, at);
		TextIterator endTextChunk(textModel, at + span);

		std::copy(beginTextChunk, endTextChunk, std::back_inserter(text));
		int32 len=text.Length();
		if(len>=1)
		{
			CAlert::InformationAlert("cell text is :"+text);
		}

	} while(false);
}


/*
void MyActiveSelectionObserver::documentTraversal(UIDRef docRef ,bool16 bAttach)
{
do
{
if(!docRef)	           {CAlert::InformationAlert("docRef failed");		break;  }

InterfacePtr<IDocument> document(docRef, UseDefaultIID());
if(!document)          {CAlert::InformationAlert("document failed");	break;	}

IDataBase* database = ::GetDataBase(document);
if(!database)          {CAlert::InformationAlert("database failed");	break;	}

InterfacePtr<ISpreadList> spreadList(document, UseDefaultIID());                       //    list of spreads
if(!spreadList)        {CAlert::InformationAlert("spreadList failed");	break;	}

// Build a list of all the graphic frames in the document
UIDList frameList(database);


int32 spreadCount = spreadList->GetSpreadCount();                        //spread count
for (int32 spreadIndex = 0; spreadIndex < spreadCount; spreadIndex++ )
{
UIDRef spreadUIDRef(database, spreadList->GetNthSpreadUID(spreadIndex));

InterfacePtr<ISpread> spread(spreadUIDRef, UseDefaultIID());
if(!spread)			{CAlert::InformationAlert("spread failed");		break;	}

// Iterate over all the pages on the spread.
int32 numberOfPages = spread->GetNumPages();
PMString noPages;
noPages.AppendNumber(numberOfPages);
CAlert::InformationAlert("No.of pages are:"+noPages);
/*
for (int32 nPage = 0; nPage < numberOfPages; nPage++ )
{
UIDList pageItemList(database);
spread->GetItemsOnPage
(
nPage, 
&pageItemList, 
kFalse, // don't include the page object itself
kFalse // don't include items that lie on the pasteboard because we only want what's on the page?  
);

// Iterate the page items and save off the UIDs of frames.
int32 pageItemListLength = pageItemList.Length();
for (int32 j = 0; j < pageItemListLength; j++ )
{
UIDRef pageItemRef = pageItemList.GetRef(j);
InterfacePtr<IGraphicFrameData> graphicFrameData(pageItemRef, UseDefaultIID());
if (graphicFrameData != nil) {
int32 absolPage = this->GlobalFromLocalPageIndex(spreadList, spreadIndex, nPage);
this->addGraphicFrameDescription(docRef,
pageItemRef,
collectFrames, 
absolPage);
}
}
} // iterate pages in spread		
*/
/*	} // interate spreads

} while(kFalse);

}*/

void MyActiveSelectionObserver::styleCharaPara()
{
	do
	{
		/*
		InterfacePtr<IDocument> document(this, UseDefaultIID());
		if (!document)						{	CAlert::InformationAlert("document Failed:");					break;	}

		IDataBase* database = ::GetDataBase(document);


		CAlert::InformationAlert("--------------222-------------------");
		//UIDRef wsRef=document->GetDocWorkSpace();
		//InterfacePtr<IWorkspace> ws(document->GetDocWorkSpace(), UseDefaultIID());
		InterfacePtr<IWorkspace> ws(GetExecutionContextSession()->QueryWorkspace());
		if(!ws)		{	CAlert::InformationAlert("ws failed");	break;	}


		InterfacePtr<IStyleNameTable> styleNameTable(ws, IID_IPARASTYLENAMETABLE);
		if(!styleNameTable)		{	CAlert::InformationAlert("styleNameTable failed");	break;	}

		int32 styleNum=styleNameTable->GetNumStyles();
		for(int32 i=0;i<styleNum;i++)
		{
		CAlert::InformationAlert("--------------333-------------------");
		UID styleUID=styleNameTable->GetNthStyle(i);
		InterfacePtr<IStyleInfo> styleInfo(database,styleUID, UseDefaultIID());
		if(!styleInfo)		{	CAlert::InformationAlert("styleInfo failed");	break;	}

		PMString paraStyleName=styleInfo->GetName();
		CAlert::InformationAlert("paraStyleName is:"+paraStyleName);
		}
		*/

		/*
		//UIDRef wsRef=document->GetDocWorkSpace();
		//InterfacePtr<IWorkspace> ws(document->GetDocWorkSpace(), UseDefaultIID());
		//InterfacePtr<IWorkspace> ws(GetExecutionContextSession()->QueryWorkspace());
		IActiveContext *context = GetExecutionContextSession()->GetActiveContext();

		IWorkspace* ws = context->GetContextWorkspace();
		if(!ws)		{	CAlert::InformationAlert("ws failed");	break;	}


		InterfacePtr<IStyleNameTable> styleNameTable(ws, IID_IPARASTYLENAMETABLE);//ICHARSTYLENAMETABLE_OBSOLETE
		//InterfacePtr<IStyleGroupManager> styleNameTable(document->GetDocWorkSpace(), IID_ICHARSTYLEGROUPMANAGER);
		if(!styleNameTable)		{	CAlert::InformationAlert("styleNameTable failed");	break;	}

		int32 styleNum=styleNameTable->GetNumStyles();
		PMString styleNumString;
		styleNumString.AppendNumber(styleNum);
		CAlert::InformationAlert("paraStyle count is:"+styleNumString);

		for(int32 i=0;i<styleNum;i++)
		{

		UID styleUID=styleNameTable->GetNthStyle(i);

		//UID rootStyleUID=styleNameTable->GetRootStyleUID();
		//InterfacePtr<IStyleInfo> styleInfo(database,styleUID, UseDefaultIID());
		InterfacePtr<IStyleInfo> styleInfo(::GetDataBase(styleNameTable),styleUID,UseDefaultIID());
		if(!styleInfo)		{	CAlert::InformationAlert("styleInfo failed");	break;	}
		bool16 res=styleInfo->IsParagraphStyle();
		if(res==kTrue)
		{
		CAlert::InformationAlert("IsParagraphStyle :");
		}

		PMString paraStyleName=styleInfo->GetName();
		CAlert::InformationAlert("paraStyleName is:"+paraStyleName);
		}
		*/

		InterfacePtr<ITextSelectionSuite> iTextSelectionSuite(fCurrentSelection, UseDefaultIID());
		if (!iTextSelectionSuite)			{	CAlert::InformationAlert("iTextSelectionSuite Failed:");		break;	}


		InterfacePtr<const IIntegratorTarget> iIntegratorTarget_text(iTextSelectionSuite, UseDefaultIID());
		if(iIntegratorTarget_text == nil)
		{	return;	}

		RangeData rangeData(0,RangeData::kLeanForward);
		UIDRef textRef;

		/*
		InterfacePtr<ILayoutSelectionSuite> iLayoutSelectionSuite(fCurrentSelection, UseDefaultIID());
		if (!iLayoutSelectionSuite)			{	CAlert::InformationAlert("compScanner Failed is :");			break;	}


		InterfacePtr<const IIntegratorTarget> iIntegratorTarget_layout(iLayoutSelectionSuite, UseDefaultIID());
		if(!iIntegratorTarget_layout)		{	CAlert::InformationAlert("iIntegratorTarget_layout failed");	break;	}

		//std::auto_ptr<IIntegratorTarget::TargetSuiteCollection> selectionSuites_layout(iIntegratorTarget_layout->GetTarget(ILayoutTarget::kDefaultIID));
		*/

		std::auto_ptr<IIntegratorTarget::TargetSuiteCollection> selectionSuites_text(iIntegratorTarget_text->GetTarget(ITextTarget::kDefaultIID));
		for(IIntegratorTarget::TargetSuiteCollection::size_type i = 0; i < selectionSuites_text->size(); i++)
		{
			ITextTarget* textTarget = (ITextTarget*)selectionSuites_text->at(i).get();
			if(!textTarget)
				continue;
			rangeData = textTarget->GetRange();
			int32 start = rangeData.Start(nil);
			int32 end = rangeData.End();
			int32 len = end-start;

			textRef = textTarget->GetTextModel();
			if(textRef == nil)
				return;


			InterfacePtr<ITextModel> textModel(textTarget->QueryTextModel());
			if (!textModel)					{	CAlert::InformationAlert("textModel Failed");					break;	}


			InterfacePtr<IAttributeStrand> ipastrand((IAttributeStrand*)(textModel->QueryStrand(kParaAttrStrandBoss,IID_IATTRIBUTESTRAND)));
			if (!ipastrand)					{	CAlert::InformationAlert("ipastrand Failed");					break;	}	

			InterfacePtr<IAttributeStrand> icastrand((IAttributeStrand*)(textModel->QueryStrand(kCharAttrStrandBoss,IID_IATTRIBUTESTRAND)));
			if (!icastrand)					{	CAlert::InformationAlert("icastrand Failed");					break;	}	

			int32 count,count2; // number of chars from start that have this style
			// let's see what style applies to this bit of text
			UID parastyle = ipastrand->GetStyleUID(start,&count);

			UID carastyle = icastrand->GetStyleUID(start,&count2);

			IActiveContext* activeContext = GetExecutionContextSession()->GetActiveContext();
			if (!activeContext)				{	CAlert::InformationAlert("activeContext Failed");					break;	}	

			IWorkspace * theWS = activeContext->GetContextWorkspace();

			InterfacePtr<IStyleInfo> paraStyleInfo(::GetDataBase(theWS),parastyle,UseDefaultIID());
			if(!paraStyleInfo)		{	CAlert::InformationAlert("paraStyleInfo failed");	break;	}

			InterfacePtr<IStyleInfo> caraStyleInfo(::GetDataBase(theWS),carastyle,UseDefaultIID());
			if(!caraStyleInfo)		{	CAlert::InformationAlert("caraStyleInfo failed");	break;	}

			PMString paraStyleName=paraStyleInfo->GetName();
			PMString caraStyleName=caraStyleInfo->GetName();
			PMString charNumbers,charNumbers2;
			charNumbers.AppendNumber(count);
			charNumbers2.AppendNumber(count2);
			CAlert::InformationAlert("paraStyleName is:"+paraStyleName+"/////"+charNumbers+"***caraStyleName is:"+caraStyleName+"\\\\\\"+charNumbers2);



			/*
			InterfacePtr<ITextModel> textModel(textTarget->QueryTextModel());
			if (!textModel)					{	CAlert::InformationAlert("textModel Failed");					break;	}

			InterfacePtr<IComposeScanner> compScanner(textModel, UseDefaultIID());
			if (!compScanner)				{	CAlert::InformationAlert("compScanner Failed");					break;	}


			IDrawingStyle* drawStyle = compScanner->GetParagraphStyleAt(rangeData.Start(nil), nil);
			if (!drawStyle)				{	CAlert::InformationAlert("drawStyle Failed");					break;	}

			InterfacePtr<ICompositionStyle> compStyle(drawStyle, UseDefaultIID());
			if (!compStyle)				{	CAlert::InformationAlert("compStyle Failed");					break;	}


			int32 dirPara = compStyle->GetParagraphDirection();
			PMString paraDirection;
			paraDirection.AppendNumber(dirPara);
			CAlert::InformationAlert("GetParagraphDirection is:"+paraDirection);
			*/
			/* // default name alert.....error
			InterfacePtr<IStyleGroupManager> paraGroupMgr(document->GetDocWorkSpace(), IID_IPARASTYLEGROUPMANAGER);
			UID styleRootUID = paraGroupMgr->GetRootStyleUID();
			if(styleRootUID==0)				{	CAlert::InformationAlert("styleRootUID Failed :");			break;	}
			else
			{
			//UIDRef styleRef = kInvalidUIDRef;


			//InterfacePtr<IStyleInfo> styleInfo(database,styleRootUID, UseDefaultIID());
			InterfacePtr<IStyleInfo> styleInfo(::GetDataBase(paraGroupMgr),styleRootUID,UseDefaultIID());
			if(!styleInfo)				{	CAlert::InformationAlert("styleInfo Failed ");			break;	}

			PMString paraStyleName=styleInfo->GetName();
			CAlert::InformationAlert("paraStyleName is:"+paraStyleName);

			}
			*/				

		}
	}while(kFalse);
}

/*
IDocument *doc = GetExecutionContextSession()->GetActiveContext()->GetContextDocument();
UIDRef ws = doc->GetDocWorkSpace();
InterfacePtr<IStyleGroupManager> paraStyleNameTable(ws, IID_IPARASTYLEGROUPMANAGER);
PMString styleName("someStyle");
UID paraStyleUID = paraStyleNameTable->FindByName(styleName);
if(paraStyleUID != kInvalidUID)
{
//We found the style
UIDRef styleRef(doc->GetDataBase(), paraStyleUID);
}

*/


void MyActiveSelectionObserver::exportSelectionHTML(IGraphicFrameData* graphicFrameData)
{
	PMString fullPath("D:\\DocWch\\GeneratedFiles\\selectionExport.html");
	IDFile targetFile(fullPath);

	PMString HTMLFormat("HTML");

	HTMLFormat.SetTranslatable(kFalse);
	IDocument* doc = Utils<ILayoutUIUtils>()->GetFrontDocument();
	InterfacePtr<IK2ServiceRegistry> k2ServiceRegistry(GetExecutionContextSession(), UseDefaultIID());
	// Look for all service providers with kExportProviderService.
	int32 exportProviderCount = k2ServiceRegistry->GetServiceProviderCount(kExportProviderService); 
	bool found = kFalse;
	//InterfacePtr<ISelectionManager> selection(Utils<ISelectionUtils>()->QueryActiveSelection());

	for (int32 exportProviderIndex = 0 ; exportProviderIndex < exportProviderCount ; exportProviderIndex++)
	{ 
		InterfacePtr<IK2ServiceProvider> k2ServiceProvider(k2ServiceRegistry->QueryNthServiceProvider(kExportProviderService,exportProviderIndex));  
		InterfacePtr<IExportProvider> exportProvider(k2ServiceProvider,IID_IEXPORTPROVIDER);
		exportProvider->ExportToFile(targetFile, doc, doc, "HTML", kFullUI);
	}
}