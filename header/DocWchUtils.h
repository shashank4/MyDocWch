//========================================================================================
//  
//  $File: //depot/devtech/nevis/plugin/source/sdksamples/docwatch/DocWchUtils.h $
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

#ifndef __DocWchUtils_h__
#define __DocWchUtils_h__

// Interface includes
#include "IDocument.h"

#include "IStoryList.h"

//#include "ITextModel.h"

/** DocWchUtils
	provides a set of utility methods for tracking which demo
	mode is active as well as formatting alerts.

	@ingroup docwatch
	
*/
class DocWchUtils
{
	public:
		static void GetDefaultDocFileHandler();
		
		/** Constructor */
		DocWchUtils() {};

		/** Methods to find out what mode we're in. */
		static bool16 QueryDocResponderMode() {return fDocResponder;};
		//static bool16 QueryTextResponderMode(){return fTextResponder;};

		/** 
		Show an information alert 
			@param document ptr to document that is subject of file action.
			@param txtMsg ptr to null terminated char array.  If it
			contains ^1, this method will replace it with the document name.
		*/
		static void DwAlert(UIDRef document, char* txtMsg);

		/** Initializes the static variables. */
		static void InitDocWchModes();

		/** Controls the document Responder mode variable. */
		static void StartDocResponderMode() { fDocResponder = kTrue; };
		static void StopDocResponderMode() { fDocResponder = kFalse; };

//		static void NewStoryCreateRespondMode(){fTextResponder=kTrue;};

	private:
		static bool16 fDocResponder;	/** Returns whether the document Responder is in use */
	
};

#endif // __DocWchUtils_h__

// End, DocWchUtils.h.


