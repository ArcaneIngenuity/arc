//Retrieves raw input and stores it.
//we maintain both passive and active functions as some languages / platforms / environments may require a mix of these.

disjunction.extensions.Keyboard = function()
{
	disjunction.core.Device.call(this, 256); //super-constructor (add params after this)
	
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

disjunction.extensions.Keyboard.prototype = new disjunction.core.Device();
disjunction.extensions.Keyboard.prototype.constructor = disjunction.extensions.Keyboard;

//from code.google.com/p/closure-library/source/browse/closure/goog/events/keycodes.js
disjunction.constants.KEY_WIN_FF_LINUX = 0;
disjunction.constants.KEY_MAC_ENTER = 3;
disjunction.constants.KEY_BACKSPACE = 8;
disjunction.constants.KEY_TAB = 9;
disjunction.constants.KEY_NUM_CENTER = 12;  // NUMLOCK on FF/Safari Mac
disjunction.constants.KEY_ENTER = 13;
disjunction.constants.KEY_SHIFT = 16;
disjunction.constants.KEY_CTRL = 17;
disjunction.constants.KEY_ALT = 18;
disjunction.constants.KEY_PAUSE = 19;
disjunction.constants.KEY_CAPS_LOCK = 20;
disjunction.constants.KEY_ESC = 27;
disjunction.constants.KEY_SPACE = 32;
disjunction.constants.KEY_PAGE_UP = 33;     // also NUM_NORTH_EAST
disjunction.constants.KEY_PAGE_DOWN = 34;   // also NUM_SOUTH_EAST
disjunction.constants.KEY_END = 35;         // also NUM_SOUTH_WEST
disjunction.constants.KEY_HOME = 36;        // also NUM_NORTH_WEST
disjunction.constants.KEY_LEFT = 37;        // also NUM_WEST
disjunction.constants.KEY_UP = 38;          // also NUM_NORTH
disjunction.constants.KEY_RIGHT = 39;       // also NUM_EAST
disjunction.constants.KEY_DOWN = 40;        // also NUM_SOUTH
disjunction.constants.KEY_PRINT_SCREEN = 44;
disjunction.constants.KEY_INSERT = 45;      // also NUM_INSERT
disjunction.constants.KEY_DELETE = 46;      // also NUM_DELETE
disjunction.constants.KEY_ZERO = 48;
disjunction.constants.KEY_ONE = 49;
disjunction.constants.KEY_TWO = 50;
disjunction.constants.KEY_THREE = 51;
disjunction.constants.KEY_FOUR = 52;
disjunction.constants.KEY_FIVE = 53;
disjunction.constants.KEY_SIX = 54;
disjunction.constants.KEY_SEVEN = 55;
disjunction.constants.KEY_EIGHT = 56;
disjunction.constants.KEY_NINE = 57;
disjunction.constants.KEY_FF_SEMICOLON = 59; // Firefox (Gecko) fires this for semicolon instead of 186
disjunction.constants.KEY_FF_EQUALS = 61; // Firefox (Gecko) fires this for equals instead of 187
disjunction.constants.KEY_FF_DASH = 173; // Firefox (Gecko) fires this for dash instead of 189
disjunction.constants.KEY_QUESTION_MARK = 63; // needs localization
disjunction.constants.KEY_A = 65;
disjunction.constants.KEY_B = 66;
disjunction.constants.KEY_C = 67;
disjunction.constants.KEY_D = 68;
disjunction.constants.KEY_E = 69;
disjunction.constants.KEY_F = 70;
disjunction.constants.KEY_G = 71;
disjunction.constants.KEY_H = 72;
disjunction.constants.KEY_I = 73;
disjunction.constants.KEY_J = 74;
disjunction.constants.KEY_K = 75;
disjunction.constants.KEY_L = 76;
disjunction.constants.KEY_M = 77;
disjunction.constants.KEY_N = 78;
disjunction.constants.KEY_O = 79;
disjunction.constants.KEY_P = 80;
disjunction.constants.KEY_Q = 81;
disjunction.constants.KEY_R = 82;
disjunction.constants.KEY_S = 83;
disjunction.constants.KEY_T = 84;
disjunction.constants.KEY_U = 85;
disjunction.constants.KEY_V = 86;
disjunction.constants.KEY_W = 87;
disjunction.constants.KEY_X = 88;
disjunction.constants.KEY_Y = 89;
disjunction.constants.KEY_Z = 90;
disjunction.constants.KEY_META = 91; // WIN_disjunction.constants.KEY_LEFT
disjunction.constants.KEY_WIN_KEY_RIGHT = 92;
disjunction.constants.KEY_CONTEXT_MENU = 93;
disjunction.constants.KEY_NUM_ZERO = 96;
disjunction.constants.KEY_NUM_ONE = 97;
disjunction.constants.KEY_NUM_TWO = 98;
disjunction.constants.KEY_NUM_THREE = 99;
disjunction.constants.KEY_NUM_FOUR = 100;
disjunction.constants.KEY_NUM_FIVE = 101;
disjunction.constants.KEY_NUM_SIX = 102;
disjunction.constants.KEY_NUM_SEVEN = 103;
disjunction.constants.KEY_NUM_EIGHT = 104;
disjunction.constants.KEY_NUM_NINE = 105;
disjunction.constants.KEY_NUM_MULTIPLY = 106;
disjunction.constants.KEY_NUM_PLUS = 107;
disjunction.constants.KEY_NUM_MINUS = 109;
disjunction.constants.KEY_NUM_PERIOD = 110;
disjunction.constants.KEY_NUM_DIVISION = 111;
disjunction.constants.KEY_F1 = 112;
disjunction.constants.KEY_F2 = 113;
disjunction.constants.KEY_F3 = 114;
disjunction.constants.KEY_F4 = 115;
disjunction.constants.KEY_F5 = 116;
disjunction.constants.KEY_F6 = 117;
disjunction.constants.KEY_F7 = 118;
disjunction.constants.KEY_F8 = 119;
disjunction.constants.KEY_F9 = 120;
disjunction.constants.KEY_F10 = 121;
disjunction.constants.KEY_F11 = 122;
disjunction.constants.KEY_F12 = 123;
disjunction.constants.KEY_NUMLOCK = 144;
disjunction.constants.KEY_SCROLL_LOCK = 145;

// OS-specific media keys like volume controls and browser controls.
disjunction.constants.KEY_FIRST_MEDIA_KEY = 166;
disjunction.constants.KEY_LAST_MEDIA_KEY = 183;

disjunction.constants.KEY_SEMICOLON = 186;            // needs localization
disjunction.constants.KEY_DASH = 189;                 // needs localization
disjunction.constants.KEY_EQUALS = 187;               // needs localization
disjunction.constants.KEY_COMMA = 188;                // needs localization
disjunction.constants.KEY_PERIOD = 190;               // needs localization
disjunction.constants.KEY_SLASH = 191;                // needs localization
disjunction.constants.KEY_APOSTROPHE = 192;           // needs localization
disjunction.constants.KEY_TILDE = 192;                // needs localization
disjunction.constants.KEY_SINGLE_QUOTE = 222;         // needs localization
disjunction.constants.KEY_OPEN_SQUARE_BRACKET = 219;  // needs localization
disjunction.constants.KEY_BACKSLASH = 220;            // needs localization
disjunction.constants.KEY_CLOSE_SQUARE_BRACKET = 221; // needs localization
disjunction.constants.KEY_WIN_KEY = 224;
disjunction.constants.KEY_MAC_FF_META = 224; // Firefox (Gecko) fires this for the meta key instead of 91
disjunction.constants.KEY_MAC_WK_CMD_LEFT = 91;  // WebKit Left Command key fired; same as META
disjunction.constants.KEY_MAC_WK_CMD_RIGHT = 93; // WebKit Right Command key fired; different from META
disjunction.constants.KEY_WIN_IME = 229;

// System-specific
// We've seen users whose machines fire this keycode at regular one
// second intervals. The common thread among these users is that
// they're all using Dell Inspiron laptops; so we suspect that this
// indicates a hardware/bios problem.
// http =//en.community.dell.com/support-forums/laptop/f/3518/p/19285957/19523128.aspx
disjunction.constants.KEY_PHANTOM = 255;