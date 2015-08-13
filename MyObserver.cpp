
#include "VCPlugInHeaders.h"
#include "CAlert.h"

// Interface includes:
#include "ISubject.h"
#include "IDocument.h"
#include "IObjectModel.h"
#include "ITextFocus.h"
#include "ITextFocusManager.h"
#include "ITextModel.h"            
#include "IStoryList.h"
#include "IObserver.h"
/*#include "ITextSelectionSuite.h" /////////////////
#include "ISelectionManager.h"
#include "ITextSelectionSuite.h"
#include "ITextTarget.h"
#include "RangeData.h"
#include "IRangeData.h"
#include "ITextFocusManager.h"
*/
//#include "IControlView.h"
//#include "IDataBase.h"
//#include "ISelectionUtils.h"
//#include "ISelectionManager.h"
//#include "IGenericTextSelectionSuite.h"
//#include "IControllingUnknown.h"
//#include "IInvalHandler.h"



// General includes:
#include "TextEditorID.h"
#include "TextID.h"
#include "CObserver.h"
#include "Utils.h"
#include "UIDList.h"
#include "CAlert.h"
//#include "RangeData.h"
//#include "NoteID.h"
//#include "InCopySharedID.h"
//#include "InCopyCoreID.h"
//#include "OpenPlaceID.h" // kEndPlaceCmdBoss


#include "textiterator.h"
#include "TextRange.h"
#include "CmdUtils.h"

namespace InDesign { class TextRange;}
//Project includes
#include "DocWchID.h"
#include "DocWchUtils.h"

int endPosition=1;

class MyObserver : public CObserver
{
public:
	/**
	Constructor.
	@param boss interface ptr from boss object on which interface is aggregated.
	*/
	MyObserver(IPMUnknown* boss);

	/**
	Destructor.
	*/		
	~MyObserver() {};

	/** Attaches the observer to the story.
	*/
	virtual void AutoAttach();

	/** Detaches the observer from the story/
	*/
	virtual void AutoDetach();

	/** All text edits are tracked through update. Updates the snapshot interface with the value of the current text edit story and index.
	@param theChange IN the change.
	@param theSubject IN the story that was being updated.
	@param protocol IN the channel the update signal is sent on.
	@param changedBy IN the agent that caused the update to be invoked.

	*/
	virtual void Update(const ClassID& theChange, ISubject* theSubject, const PMIID& protocol, void* changedBy);
	//virtual void Update();

	WideString GetWideStringFromTextRange();                  //(const InDesign::TextRange& range);

};

/*
*/
CREATE_PMINTERFACE(MyObserver, kMyObserverImpl)


/* Constructor
*/
MyObserver::MyObserver(IPMUnknown* boss): CObserver(boss)
{
}


/* AutoAttach
*/
void MyObserver::AutoAttach()
{
	//CAlert::InformationAlert("AutoAttach");
	do
	{
		InterfacePtr<ISubject> subject(this, IID_ISUBJECT);
		if (subject != nil)
		{
			if (subject->IsAttached(ISubject::kRegularAttachment,this, IID_ITEXTMODEL,IID_IMYOBSERVER) == kFalse)
			{
				subject->AttachObserver(ISubject::kRegularAttachment,this, IID_ITEXTMODEL,IID_IMYOBSERVER);
			}
		}
	} while(kFalse);
}


/* AutoDetach
*/
void MyObserver::AutoDetach()
{
	//CAlert::InformationAlert("AutoDttach");
	do
	{
		InterfacePtr<ISubject> subject(this, UseDefaultIID());
		if (subject != nil)
		{
			if (subject->IsAttached(ISubject::kRegularAttachment,this, IID_ITEXTMODEL,IID_IMYOBSERVER) == kTrue)
			{
				subject->DetachObserver(ISubject::kRegularAttachment,this, IID_ITEXTMODEL,IID_IMYOBSERVER);
			}
		}

	} while(kFalse);
}



/* Update
*/
void MyObserver::Update(const ClassID& theChange, ISubject* theSubject, const PMIID& protocol, void* changedBy)
{
	/*if (protocol == IID_ITEXTMODEL)
	{
		InterfacePtr<IObjectModel> iObjectModel(GetExecutionContextSession(), UseDefaultIID());
		TRACEFLOW(kDocWchPluginName,"MyObserver::Update, pName %s cName %s subject %s\n", iObjectModel->GetIDName(theChange),iObjectModel->GetIDName(protocol),iObjectModel->GetIDName(::GetClass(theSubject)));
		
		ClassID changeClassID = theChange.Get();

		UIDRef storyUIDRef(::GetUIDRef(this));

		InterfacePtr<IDocument>	iDocument(storyUIDRef.GetDataBase(), storyUIDRef.GetDataBase()->GetRootUID(), UseDefaultIID());
		if (iDocument == nil){
			ASSERT_FAIL("MyObserver::Update - no document associated with story?");
			return;
		}
		do {
			if (changeClassID == kEndPlaceCmdBoss){
				// if this model is subject to a place, and we are currently the cached
				// story, we need to reset the cache (last edited makes no sense in this condition)
				InterfacePtr<IGTTxtEdtSnapshotInterface> iGTTxtEdtSnapshotInterface(iDocument,UseDefaultIID());
				if (iGTTxtEdtSnapshotInterface == nil){
					break;
				}
				UIDRef currentStoryUIDRef = iGTTxtEdtSnapshotInterface->GetStory();
				if (currentStoryUIDRef == storyUIDRef){
					iGTTxtEdtSnapshotInterface->SetStoryAndIndex(UIDRef(UIDRef::gNull),0);
				}

			}
			//Observe changes to these cmd bosses for changes to text.
			else if (changeClassID == kDeleteTextCmdBoss ||
				changeClassID == kInsertTextCmdBoss ||
				changeClassID == kReplaceTextCmdBoss ||
				changeClassID == kMultiReplaceTextCmdBoss)
			{
				TextIndex focusStart = kInvalidTextIndex;

				InterfacePtr<IStoryList> iStoryList(iDocument,UseDefaultIID());
				if (iStoryList == nil){
					ASSERT_FAIL("");
					break;
				}

				// make sure it is a user accessible story before we update the cache
				int32 storyIndex = iStoryList->GetUserAccessibleStoryIndex(storyUIDRef.GetUID());
				// only update the UDI if the story is accessible
				if (storyIndex == -1){
					break; // this is not an error condition.
				}

				InterfacePtr<ITextModel> iTextModel(storyUIDRef,UseDefaultIID());
				if (iTextModel == nil){
					ASSERT_FAIL("MyObserver::Update - no story for change?");
					break;
				}

				InterfacePtr<ITextFocusManager> iTextFocusManager(iTextModel,UseDefaultIID());
				if (iTextFocusManager == nil){
					ASSERT_FAIL("MyObserver::Update - no text focus manager for focus?");
					break;
				}

				// with the focus manager, we need to find the focus that is for the text selection
				// ust in case more than a single focus exists
				ITextFocus* iTextFocus=nil;
				for (int32 loop = 0;loop < iTextFocusManager->GetFocusCount();loop++){
					InterfacePtr<ITextFocus > nthTextFocus(iTextFocusManager->QueryNthFocus(loop));
					if (::GetClass(nthTextFocus) == kTextSelectionFocusBoss){
						iTextFocus = nthTextFocus;
					}
				}
				InterfacePtr<ITextFocus> iSelectionTextFocus(iTextFocus, UseDefaultIID());
				if (iSelectionTextFocus != nil){
					focusStart = iSelectionTextFocus->GetStart(nil);
				}


				InterfacePtr<IGTTxtEdtSnapshotInterface> iGTTxtEdtSnapshotInterface(iDocument,UseDefaultIID());
				if (iGTTxtEdtSnapshotInterface == nil || focusStart == kInvalidTextIndex){
					break;
				}
				iGTTxtEdtSnapshotInterface->SetStoryAndIndex(storyUIDRef,focusStart);
				TRACEFLOW(kGTTxtEdtPluginName,"Text story observer range == %d\n", focusStart);
			}
		}while (kFalse);
	}*/
	if(protocol==IID_ITEXTMODEL)
	{
		//CAlert::InformationAlert("Did you changed something...?");
		//UIDRef storyUIDRef(::GetUIDRef(this));
		//
		//InterfacePtr<IDocument>	iDocument(storyUIDRef.GetDataBase(), storyUIDRef.GetDataBase()->GetRootUID(), UseDefaultIID());
		//
		//TextIndex focusStart = kInvalidTextIndex;
		//
		//InterfacePtr<IStoryList> iStoryList(iDocument,UseDefaultIID());
		//
		//int32 storyIndex = iStoryList->GetUserAccessibleStoryIndex(storyUIDRef.GetUID());
		//
		//
		//ISelectionManager* iSelectionManager = Utils<ISelectionUtils>()->GetActiveSelection();
		//
		//InterfacePtr<ITextSelectionSuite> iTextSelectionSuite(iSelectionManager, UseDefaultIID());
		//
		//
		/*if (iTableSuite && iTableSuite->CanGetCellWidth()) 
		{
			PMReal cellWidth = iTableSuite->GetCellWidth();
			// ...
		}
		*/

		WideString result= GetWideStringFromTextRange();                    
		CAlert::InformationAlert("Oh Yeah...you entered :"+result);
	}
}


WideString MyObserver::GetWideStringFromTextRange()                     //(const InDesign::TextRange& range)
{
	WideString result;
	do 
	{
			UIDRef storyUIDRef(::GetUIDRef(this));
			InterfacePtr<ITextModel> textModel(storyUIDRef,UseDefaultIID());
			/*//
			CAlert::InformationAlert("--------1-------");
			InterfacePtr<ITextTarget> textTarget(this, UseDefaultIID());
			CAlert::InformationAlert("---------2--------");
			//InterfacePtr<ITextModel> textModel(textTarget->QueryTextModel ());
			InterfacePtr<ITextModel> textModel(storyUIDRef,UseDefaultIID());
			CAlert::InformationAlert("---------3--------");
			InterfacePtr<ITextFocus> textFocus(textTarget->QueryTextFocus());
			
			CAlert::InformationAlert("---------4--------");
			/*
			InterfacePtr<ITextFocusManager> iTextFocusManager(textModel,UseDefaultIID());

			ITextFocus* iTextFocus=nil;
			for (int32 loop = 0;loop < iTextFocusManager->GetFocusCount();loop++)
			{
				InterfacePtr<ITextFocus > nthTextFocus(iTextFocusManager->QueryNthFocus(loop));
				if (::GetClass(nthTextFocus) == kTextSelectionFocusBoss)
				{
					iTextFocus = nthTextFocus;
				}
			}
			InterfacePtr<ITextFocus> iTextSelectionFocus(textModel,UseDefaultIID());
			RangeData range=iTextSelectionFocus->GetCurrentRange();
			*/
			
			//RangeData range= ITextFocus::GetCurrentRange();
			//
			//TextIterator begin(textModel,textFocus->GetStart(nil));
			//CAlert::InformationAlert("---------5--------");
			//TextIterator end(textModel,textFocus->GetEnd());                   
			
			TextIterator begin(textModel,0);
			TextIterator end(textModel,endPosition);

			for (TextIterator iter = begin; iter != end; iter++) 
			{
				const UTF32TextChar characterCode = *iter;
				result.Append(characterCode);
			}	
			endPosition++;
	} while(false);
	return result;
}



