#ifndef RefalRTS_COMMANDS_H_
#define RefalRTS_COMMANDS_H_


namespace refalrts {

enum iCmd { /*NumberFromOpcode:Cmd+ic;Alg+Left,Alg+Right,Alg+Term*/
  icProfileFunction = 0,
  icLoadConstants = 1,
  icIssueMemory = 2,
  icReserveBacktrackStack = 3,
  icOnFailGoTo = 4,
  icProfilerStopSentence = 5,
  icInitB0 = 6,
  icInitB0_Lite = 7,
  icCharLeft = 8,
  icCharRight = 9,
  icCharTerm = 10,
  icCharSaveLeft = 11,
  icCharSaveRight = 12,
  icNumberLeft = 13,
  icNumberRight = 14,
  icNumberTerm = 15,
  icNumberSaveLeft = 16,
  icNumberSaveRight = 17,
  icHugeNumLeft = 18,
  icHugeNumRight = 19,
  icHugeNumTerm = 20,
  icHugeNumSaveLeft = 21,
  icHugeNumSaveRight = 22,
  icNameLeft = 23,
  icNameRight = 24,
  icNameTerm = 25,
  icNameSaveLeft = 26,
  icNameSaveRight = 27,
  icIdentLeft = 28,
  icIdentRight = 29,
  icIdentTerm = 30,
  icIdentSaveLeft = 31,
  icIdentSaveRight = 32,
  icBracketsLeft = 33,
  icBracketsRight = 34,
  icBracketsTerm = 35,
  icBracketsSaveLeft = 36,
  icBracketsSaveRight = 37,
  icADTLeft = 38,
  icADTRight = 39,
  icADTTerm = 40,
  icADTSaveLeft = 41,
  icADTSaveRight = 42,
  icADTSaveTerm = 43,
  icCallSaveLeft = 44,
  icEmpty = 45,
  icNotEmpty = 46,
  icsVarLeft = 47,
  icsVarRight = 48,
  icsVarTerm = 49,
  ictVarLeft = 50,
  ictVarRight = 51,
  ictVarSaveLeft = 52,
  ictVarSaveRight = 53,
  iceRepeatedLeft = 54,
  iceRepeatedRight = 55,
  icsRepeatedLeft = 56,
  icsRepeatedRight = 57,
  icsRepeatedTerm = 58,
  ictRepeatedLeft = 59,
  ictRepeatedRight = 60,
  ictRepeatedTerm = 61,
  ictRepeatedSaveLeft = 62,
  ictRepeatedSaveRight = 63,
  icEPrepare = 64,
  icEStart = 65,
  icSave = 66,
  icResetAllocator = 67,
  icSetResArgBegin = 68,
  icSetResRightEdge = 69,
  icSetRes = 70,
  icCopyEVar = 71,
  icCopySTVar = 72,
  icReinitSVar = 73,
  /*+WORDS:Allocate,Reinit,Update*/
  /*+WORDS:El+Char,El+Name,El+Number,El+HugeNumber,El+Ident,El+Bracket,El+String*/
  /*+WORDS:El+ClosureHead,El+UnwrappedClosure*/
  icAllocateChar = 74,
  icAllocateName = 75,
  icAllocateNumber = 76,
  icAllocateHugeNumber = 77,
  icAllocateIdent = 78,
  icAllocateBracket = 79,
  icAllocateString = 80,
  icAllocateClosureHead = 113,
  icAllocateUnwrappedClosure = 114,
  icReinitChar = 81,
  icReinitName = 82,
  icReinitNumber = 83,
  icReinitHugeNumber = 84,
  icReinitIdent = 85,
  icReinitBracket = 86,
  icReinitClosureHead = 87,
  icReinitUnwrappedClosure = 88,
  icUpdateChar = 89,
  icUpdateName = 90,
  icUpdateNumber = 91,
  icUpdateHugeNumber = 92,
  icUpdateIdent = 93,
  icLinkBrackets = 94,
  icPushStack = 95,
  icWrapClosure = 110,
  icSpliceElem = 96,
  icSpliceEVar = 97,
  icSpliceSTVar = 98,
  icSpliceRange = 99,
  icSpliceTile = 100,
  icSpliceToFreeList = 101,
  icNextStep = 102,
  icTrashLeftEdge = 103,
  icTrash = 104,
  icFail = 105,
  icFetchSwapHead = 106,
  icFetchSwapInfoBounds = 107,
  icSwapSave = 108,
  icPerformNative = 109,
  icScale = 111,
  icVariableDebugOffset = 112,
  icSpliceToFreeList_Range = 115,
  icPushState = 116,
  icPopState = 117,
  icMainLoopReturnSuccess = 118,
};

enum BracketType { /*NumberFromBracket:El+ib;*/
  ibOpenADT = 0,
  ibOpenBracket = 1,
  ibOpenCall = 2,
  ibCloseADT = 3,
  ibCloseBracket = 4,
  ibCloseCall = 5,
};


enum BlockType { /*BlockTypeNumber:cBlockType;*/
  cBlockTypeStart = 1,
  cBlockTypeConstTable = 2,
  cBlockTypeRefalFunction = 3,
  cBlockTypeNativeFunction = 4,
  cBlockTypeEmptyFunction = 5,
  cBlockTypeSwap = 6,
  cBlockTypeReference = 7,
  cBlockTypeConditionRasl = 8,
  cBlockTypeConditionNative = 9,
  cBlockTypeIncorporated = 10,
  cBlockTypeUnitName = 11,
  cBlockTypeMetatable = 12,
};

} // namespace refalrts


#endif  // RefalRTS_COMMANDS_H_
