//---------------------------------------------------------------------------

#pragma hdrstop

#include "message.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

TModalResult Message(const UnicodeString &msg, EMsgType msgType, const UnicodeString &title, const TMsgDlgButtons &buttons)
{
    if (msgType == EMsgType::mtExit)
    {
        Application->Terminate();
        return mrNone;
    }
    TMsgDlgType dlgType = TMsgDlgType::mtCustom;
    switch (msgType)
    {
        case EMsgType::mtInfo:    dlgType = TMsgDlgType::mtInformation;   break;
        case EMsgType::mtError:   dlgType = TMsgDlgType::mtError;         break;
        case EMsgType::mtWarning: dlgType = TMsgDlgType::mtWarning;       break;
        case EMsgType::mtConfirm: dlgType = TMsgDlgType::mtConfirmation;  break;
        default:                  dlgType = TMsgDlgType::mtCustom;        break;
    }

    TModalResult modalResult = mrNone;
    TDialogService::MessageDialog(msg, dlgType, buttons, TMsgDlgBtn::mbOK, 0,
        [&modalResult](TModalResult Result) { modalResult = Result; });

    while (modalResult == mrNone)
        Application->ProcessMessages();

    return modalResult;
}

