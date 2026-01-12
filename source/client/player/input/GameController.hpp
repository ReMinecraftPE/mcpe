#pragma once

class GameController
{
public:
	typedef unsigned int ID;
	typedef unsigned int NativeButtonID;
    typedef unsigned int StickID;
    typedef unsigned int TriggerID;

public:
    enum ConnectionState
    {
        STATE_CONNECTED,
        STATE_DISCONNECTED
    };
	enum ButtonState
	{
		BTN_STATE_UP,
		BTN_STATE_DOWN
	};
    enum StickState
    {
        STK_STATE_UNKNOWN
    };
	enum EngineButtonID
	{
        BUTTON_A,
        BUTTON_B,
        BUTTON_X,
        BUTTON_Y,
        BUTTON_DPAD_UP,
        BUTTON_DPAD_DOWN,
        BUTTON_DPAD_LEFT,
        BUTTON_DPAD_RIGHT,
        BUTTON_LEFTSTICK,
        BUTTON_RIGHTSTICK,
        BUTTON_LEFTSHOULDER,
        BUTTON_RIGHTSHOULDER,
        BUTTON_BACK,
        BUTTON_START,

        BUTTON_GUIDE,
        BUTTON_MISC1,    /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
        BUTTON_PADDLE1,  /* Xbox Elite paddle P1 (upper left, facing the back) */
        BUTTON_PADDLE2,  /* Xbox Elite paddle P3 (upper right, facing the back) */
        BUTTON_PADDLE3,  /* Xbox Elite paddle P2 (lower left, facing the back) */
        BUTTON_PADDLE4,  /* Xbox Elite paddle P4 (lower right, facing the back) */
        BUTTON_TOUCHPAD, /* PS4/PS5 touchpad button */
        BUTTON_MAX
	};
    enum EventType
    {
        EVENT_BUTTON,
        EVENT_STICK,
        EVENT_TRIGGER,
        EVENT_JOIN_GAME,
        EVENT_CHANGE_USER,
        EVENT_CONNECTION_STATE_CHANGE,
    };

public:
    struct ButtonEvent
    {
        EngineButtonID id;
        ButtonState state;
        bool bAllowRemapping;
    };
    struct StickEvent
    {
        StickID id;
        StickState state;
        float x;
		float y;
    };
    struct TriggerEvent
    {
        TriggerID id;
        float magnitude;
    };
    struct ConnectionStateEvent
    {
        ConnectionState state;
    };
    struct JoinEvent
    {
        bool bIsConfirmation;
    };
    struct ChangeUserEvent
    {
        bool bRestrictToControllerIdChange;
    };
    union EventData
    {
        ButtonEvent buttonEvent;
        StickEvent stickEvent;
        TriggerEvent triggerEvent;
        ConnectionStateEvent connectionEvent;
        JoinEvent joinEvent;
        ChangeUserEvent changeUserEvent;
    };
    struct Event
    {
        EventType type;
        EventData data;
        ID controllerId;
    };
};
