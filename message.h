#ifndef messageH
#define messageH

#include <System.SysUtils.hpp>
#include <FMX.Dialogs.hpp>
#include <FMX.Types.hpp>
#include <FMX.DialogService.hpp>

enum class EMsgType {
    mtInfo,
    mtError,
    mtWarning,
    mtConfirm,
    mtExit
};

TModalResult Message(const UnicodeString &msg, EMsgType msgType, const UnicodeString &title, const TMsgDlgButtons &buttons = TMsgDlgButtons() << TMsgDlgBtn::mbOK);

template<typename... Args>
inline TModalResult MessageFmt(const UnicodeString &fmt, EMsgType msgType, const UnicodeString &title, const TMsgDlgButtons &buttons, Args... args)
{
    return Message(Format(fmt, ARRAYOFCONST((args...))), msgType, title, buttons);
}

inline TModalResult ErrorMessage(const UnicodeString &msg)
{
    return Message(msg, EMsgType::mtError, L"Error");
}

template<typename... Args>
inline TModalResult ErrorMessage(const UnicodeString &fmt, Args... args)
{
    return MessageFmt(fmt, EMsgType::mtError, L"Error", TMsgDlgButtons() << TMsgDlgBtn::mbOK, args...);
}

inline TModalResult InfoMessage(const UnicodeString &msg)
{
    return Message(msg, EMsgType::mtInfo, L"Information");
}

template<typename... Args>
inline TModalResult InfoMessage(const UnicodeString &fmt, Args... args)
{
    return MessageFmt(fmt, EMsgType::mtInfo, L"Information", TMsgDlgButtons() << TMsgDlgBtn::mbOK, args...);
}

inline TModalResult AlertMessage(const UnicodeString &msg)
{
    return Message(msg, EMsgType::mtWarning, L"Warning");
}

template<typename... Args>
inline TModalResult AlertMessage(const UnicodeString &fmt, Args... args)
{
    return MessageFmt(fmt, EMsgType::mtWarning, L"Warning", TMsgDlgButtons() << TMsgDlgBtn::mbOK, args...);
}

inline TModalResult ConfirmMessage(const UnicodeString &msg)
{
    return Message(msg, EMsgType::mtConfirm, L"Confirm", TMsgDlgButtons() << TMsgDlgBtn::mbYes << TMsgDlgBtn::mbNo);
}

template<typename... Args>
inline TModalResult ConfirmMessage(const UnicodeString &fmt, Args... args)
{
    return MessageFmt(fmt, EMsgType::mtConfirm, L"Confirm", TMsgDlgButtons() << TMsgDlgBtn::mbYes << TMsgDlgBtn::mbNo, args...);
}

#endif

