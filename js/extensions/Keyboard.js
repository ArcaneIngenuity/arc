//Retrieves raw input and stores it.
//we maintain both passive and active functions as some languages / platforms / environments may require a mix of these.

Keyboard = function()
{
	InputDevice.call(this, 256); //super-constructor (add params after this)
	
	this.receive = function(event)
	{
		//console.log(event);
		var deviceName = event.type.substring(0, 3);
		if (deviceName === 'key')
		{
			var inputType = event.type.substring(3, 5);
				
			if (inputType === 'do') //down
			{
				var channel = this.channels[event.keyCode];
				var oldValue = channel.value;
				channel.value = 1;
				channel.delta = channel.value - oldValue;
			}
			if (inputType === 'up')
			{
				var channel = this.channels[event.keyCode];
				var oldValue = channel.value;
				channel.value = 0;
				channel.delta = channel.value - oldValue;
			}
		}
	}
}

Keyboard.prototype = new InputDevice();
Keyboard.prototype.constructor = Keyboard;

//from code.google.com/p/closure-library/source/browse/closure/goog/events/keycodes.js
KEY_WIN_FF_LINUX = 0;
KEY_MAC_ENTER = 3;
KEY_BACKSPACE = 8;
KEY_TAB = 9;
KEY_NUM_CENTER = 12;  // NUMLOCK on FF/Safari Mac
KEY_ENTER = 13;
KEY_SHIFT = 16;
KEY_CTRL = 17;
KEY_ALT = 18;
KEY_PAUSE = 19;
KEY_CAPS_LOCK = 20;
KEY_ESC = 27;
KEY_SPACE = 32;
KEY_PAGE_UP = 33;     // also NUM_NORTH_EAST
KEY_PAGE_DOWN = 34;   // also NUM_SOUTH_EAST
KEY_END = 35;         // also NUM_SOUTH_WEST
KEY_HOME = 36;        // also NUM_NORTH_WEST
KEY_LEFT = 37;        // also NUM_WEST
KEY_UP = 38;          // also NUM_NORTH
KEY_RIGHT = 39;       // also NUM_EAST
KEY_DOWN = 40;        // also NUM_SOUTH
KEY_PRINT_SCREEN = 44;
KEY_INSERT = 45;      // also NUM_INSERT
KEY_DELETE = 46;      // also NUM_DELETE
KEY_ZERO = 48;
KEY_ONE = 49;
KEY_TWO = 50;
KEY_THREE = 51;
KEY_FOUR = 52;
KEY_FIVE = 53;
KEY_SIX = 54;
KEY_SEVEN = 55;
KEY_EIGHT = 56;
KEY_NINE = 57;
KEY_FF_SEMICOLON = 59; // Firefox (Gecko) fires this for semicolon instead of 186
KEY_FF_EQUALS = 61; // Firefox (Gecko) fires this for equals instead of 187
KEY_FF_DASH = 173; // Firefox (Gecko) fires this for dash instead of 189
KEY_QUESTION_MARK = 63; // needs localization
KEY_A = 65;
KEY_B = 66;
KEY_C = 67;
KEY_D = 68;
KEY_E = 69;
KEY_F = 70;
KEY_G = 71;
KEY_H = 72;
KEY_I = 73;
KEY_J = 74;
KEY_K = 75;
KEY_L = 76;
KEY_M = 77;
KEY_N = 78;
KEY_O = 79;
KEY_P = 80;
KEY_Q = 81;
KEY_R = 82;
KEY_S = 83;
KEY_T = 84;
KEY_U = 85;
KEY_V = 86;
KEY_W = 87;
KEY_X = 88;
KEY_Y = 89;
KEY_Z = 90;
KEY_META = 91; // WIN_KEY_LEFT
KEY_WIN_KEY_RIGHT = 92;
KEY_CONTEXT_MENU = 93;
KEY_NUM_ZERO = 96;
KEY_NUM_ONE = 97;
KEY_NUM_TWO = 98;
KEY_NUM_THREE = 99;
KEY_NUM_FOUR = 100;
KEY_NUM_FIVE = 101;
KEY_NUM_SIX = 102;
KEY_NUM_SEVEN = 103;
KEY_NUM_EIGHT = 104;
KEY_NUM_NINE = 105;
KEY_NUM_MULTIPLY = 106;
KEY_NUM_PLUS = 107;
KEY_NUM_MINUS = 109;
KEY_NUM_PERIOD = 110;
KEY_NUM_DIVISION = 111;
KEY_F1 = 112;
KEY_F2 = 113;
KEY_F3 = 114;
KEY_F4 = 115;
KEY_F5 = 116;
KEY_F6 = 117;
KEY_F7 = 118;
KEY_F8 = 119;
KEY_F9 = 120;
KEY_F10 = 121;
KEY_F11 = 122;
KEY_F12 = 123;
KEY_NUMLOCK = 144;
KEY_SCROLL_LOCK = 145;

// OS-specific media keys like volume controls and browser controls.
KEY_FIRST_MEDIA_KEY = 166;
KEY_LAST_MEDIA_KEY = 183;

KEY_SEMICOLON = 186;            // needs localization
KEY_DASH = 189;                 // needs localization
KEY_EQUALS = 187;               // needs localization
KEY_COMMA = 188;                // needs localization
KEY_PERIOD = 190;               // needs localization
KEY_SLASH = 191;                // needs localization
KEY_APOSTROPHE = 192;           // needs localization
KEY_TILDE = 192;                // needs localization
KEY_SINGLE_QUOTE = 222;         // needs localization
KEY_OPEN_SQUARE_BRACKET = 219;  // needs localization
KEY_BACKSLASH = 220;            // needs localization
KEY_CLOSE_SQUARE_BRACKET = 221; // needs localization
KEY_WIN_KEY = 224;
KEY_MAC_FF_META = 224; // Firefox (Gecko) fires this for the meta key instead of 91
KEY_MAC_WK_CMD_LEFT = 91;  // WebKit Left Command key fired; same as META
KEY_MAC_WK_CMD_RIGHT = 93; // WebKit Right Command key fired; different from META
KEY_WIN_IME = 229;

// System-specific
// We've seen users whose machines fire this keycode at regular one
// second intervals. The common thread among these users is that
// they're all using Dell Inspiron laptops; so we suspect that this
// indicates a hardware/bios problem.
// http =//en.community.dell.com/support-forums/laptop/f/3518/p/19285957/19523128.aspx
KEY_PHANTOM = 255;