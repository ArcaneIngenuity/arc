//TODO rewrite all functions to use int rather than string keys.
//anything assigning (eg. new axis) should return an int which may be assigned to an appropriately named variable.

//singleton
Input = function(domElement)
{
	this.buttonNamesByEventCode =
	{
		0: 'LEFT',
		1: 'MIDDLE',
		2: 'RIGHT'
	},
	this.codes =
	{
		//from https://code.google.com/p/closure-library/source/browse/closure/goog/events/keycodes.js
		WIN_KEY_FF_LINUX: 0,
		MAC_ENTER: 3,
		BACKSPACE: 8,
		TAB: 9,
		NUM_CENTER: 12,  // NUMLOCK on FF/Safari Mac
		ENTER: 13,
		SHIFT: 16,
		CTRL: 17,
		ALT: 18,
		PAUSE: 19,
		CAPS_LOCK: 20,
		ESC: 27,
		SPACE: 32,
		PAGE_UP: 33,     // also NUM_NORTH_EAST
		PAGE_DOWN: 34,   // also NUM_SOUTH_EAST
		END: 35,         // also NUM_SOUTH_WEST
		HOME: 36,        // also NUM_NORTH_WEST
		LEFT: 37,        // also NUM_WEST
		UP: 38,          // also NUM_NORTH
		RIGHT: 39,       // also NUM_EAST
		DOWN: 40,        // also NUM_SOUTH
		PRINT_SCREEN: 44,
		INSERT: 45,      // also NUM_INSERT
		DELETE: 46,      // also NUM_DELETE
		ZERO: 48,
		ONE: 49,
		TWO: 50,
		THREE: 51,
		FOUR: 52,
		FIVE: 53,
		SIX: 54,
		SEVEN: 55,
		EIGHT: 56,
		NINE: 57,
		FF_SEMICOLON: 59, // Firefox (Gecko) fires this for semicolon instead of 186
		FF_EQUALS: 61, // Firefox (Gecko) fires this for equals instead of 187
		FF_DASH: 173, // Firefox (Gecko) fires this for dash instead of 189
		QUESTION_MARK: 63, // needs localization
		A: 65,
		B: 66,
		C: 67,
		D: 68,
		E: 69,
		F: 70,
		G: 71,
		H: 72,
		I: 73,
		J: 74,
		K: 75,
		L: 76,
		M: 77,
		N: 78,
		O: 79,
		P: 80,
		Q: 81,
		R: 82,
		S: 83,
		T: 84,
		U: 85,
		V: 86,
		W: 87,
		X: 88,
		Y: 89,
		Z: 90,
		META: 91, // WIN_KEY_LEFT
		WIN_KEY_RIGHT: 92,
		CONTEXT_MENU: 93,
		NUM_ZERO: 96,
		NUM_ONE: 97,
		NUM_TWO: 98,
		NUM_THREE: 99,
		NUM_FOUR: 100,
		NUM_FIVE: 101,
		NUM_SIX: 102,
		NUM_SEVEN: 103,
		NUM_EIGHT: 104,
		NUM_NINE: 105,
		NUM_MULTIPLY: 106,
		NUM_PLUS: 107,
		NUM_MINUS: 109,
		NUM_PERIOD: 110,
		NUM_DIVISION: 111,
		F1: 112,
		F2: 113,
		F3: 114,
		F4: 115,
		F5: 116,
		F6: 117,
		F7: 118,
		F8: 119,
		F9: 120,
		F10: 121,
		F11: 122,
		F12: 123,
		NUMLOCK: 144,
		SCROLL_LOCK: 145,

		// OS-specific media keys like volume controls and browser controls.
		FIRST_MEDIA_KEY: 166,
		LAST_MEDIA_KEY: 183,

		SEMICOLON: 186,            // needs localization
		DASH: 189,                 // needs localization
		EQUALS: 187,               // needs localization
		COMMA: 188,                // needs localization
		PERIOD: 190,               // needs localization
		SLASH: 191,                // needs localization
		APOSTROPHE: 192,           // needs localization
		TILDE: 192,                // needs localization
		SINGLE_QUOTE: 222,         // needs localization
		OPEN_SQUARE_BRACKET: 219,  // needs localization
		BACKSLASH: 220,            // needs localization
		CLOSE_SQUARE_BRACKET: 221, // needs localization
		WIN_KEY: 224,
		MAC_FF_META: 224, // Firefox (Gecko) fires this for the meta key instead of 91
		MAC_WK_CMD_LEFT: 91,  // WebKit Left Command key fired, same as META
		MAC_WK_CMD_RIGHT: 93, // WebKit Right Command key fired, different from META
		WIN_IME: 229,

		// We've seen users whose machines fire this keycode at regular one
		// second intervals. The common thread among these users is that
		// they're all using Dell Inspiron laptops, so we suspect that this
		// indicates a hardware/bios problem.
		// http://en.community.dell.com/support-forums/laptop/f/3518/p/19285957/19523128.aspx
		PHANTOM: 255,
		
		
		//this allows us to keep all in one object, and single-byte index into this table
		MOUSE_X: 230,
		MOUSE_Y: 231,
		//MOUSE_DX: 232,
		//MOUSE_DY: 233,
		MOUSE_BUTTON_LEFT: 234,
		MOUSE_BUTTON_MIDDLE: 235,
		MOUSE_BUTTON_RIGHT: 236,
		MOUSE_WHEEL: 237,
	};

	this.domElement = undefined;
	this.raw = new Array(256); //...import via init
	this.buttonSpecifications = {};
	this.axisSpecifications = {};
	this.axis = {}; //continuous motion, twin inputs as polar opposites
	this.button = {}; //one hit (holding down makes no difference, must be released first)

	this.viewportWidth = undefined;
	this.viewportHeight = undefined;
	this.mouseMovedThisUpdate = false;
	

	//assigns a new axis and returns the index it is at (can store as int in classes using it)
	//pass the names of various raw inputs for range, pos, neg
	//remapper remaps the range; this includes the responsibility for remapping to range if delta exceeds some max or min value!
	//isDelta denotes whether incoming data is delta (eg. mouse under pointer lock) as opposed to absolute (eg. keys, ordinary mouse)
	this.specifyAxis = function(name, source, remap)
	{
		if (!remap) remap = this.remapPassthrough;
		
		this.axisSpecifications[name] = { 'source': source, 'remapper': remap };
		this.axis[name] = 
		{
			abs: 0,
			absRaw: 0, //a necessity to reconstruct deltas, absolutes etc. for different types of input, given that remapping is a destructive, one-way process
			delta: 0,
		}
		
		//return index; for C, return index as there is no name!
	}

	//assigns one hit keys, no continuous feedback (for which we use axis or raw)
	this.specifyButton = function(name, code)
	{
		this.buttonSpecifications[name] = {code: code};
		this.button[name] = 
		{
			//abs: 0,
			//delta: 0,
			triggered: false
		}
	}

	//null pattern for remapping
	this.remapPassthrough = function(value) //STATIC
	{
		return value;
	}

	this.clearAbstractDeltas = function()
	{
		/*
		if (this.mouseMovedThisUpdate)
			this.mouseMovedThisUpdate = false;
		*/
		//this.raw[this.codes.MOUSE_X].delta = 0;
		//this.raw[this.codes.MOUSE_Y].delta = 0;

		for (var name in this.button)
		{
			this.button[name].triggered = false;
		}
	}
	
	this.clearRawDeltas = function()
	{
		for (var code in this.raw)
		{
			this.raw[code].delta = 0;
		}
	}

	this.updateButtons = function()
	{
		for (var name in this.buttonSpecifications)
		{
			var buttonSpecification = this.buttonSpecifications[name]; //no range
			var button = this.button[name]; //no range
			var valueRaw = this.raw[buttonSpecification.code];
			//TDP.log('raw', valueRaw);
			
			if (valueRaw.delta > 0) //TODO ...or whatever the expected delta is. just do not make != 0, since only one delta is hit, its opposite is release.
				this.button[name].triggered = true;
				
			/*
			
			if (buttonSpecification.triggered) //if it's triggered
			{
				//ensure that it's no longer active (this conditional block can only evaluate true after the first run)
				this.button[name].abs = 0; //TODO ...or whatever it's neg value is? zero should be fine.
				
				if (!this.raw[buttonSpecification.code]) //reset trigger on key release
					buttonSpecification.triggered = false;
			}
			else //if it isn't triggered
			{
				if (valueRaw.delta == 1) //trigger only if key just hit
				{
					this.button[name] = valueRaw;
					buttonSpecification.triggered = true;
				}
				//else do nothing
			}
			*/
			
		}
	}

	this.updateAxes = function()
	{
		//if ( !this.options.enabled )
		//	return;

		//update higher level input (axes etc.) from raw;
		//all remains as raw for access, anyway.
		for (var name in this.axisSpecifications)
		{
			
			var axisSpecification = this.axisSpecifications[name]; //no range
			var source = axisSpecification.source;
			var valueRaw;

			if (source.hasOwnProperty('code')) //if axis uses an existing value
			{
				valueRaw = this.raw[source.code].abs;
			}
			else //if axis value must be calculated from two inputs
			{
				valueRaw = this.raw[source.posCode].abs - this.raw[source.negCode].abs;
			}
			
			var absOld = this.axis[name].abs;
			var absRawOld = this.axis[name].absRaw;
			var absRaw = valueRaw;
			var abs = this.axis[name].abs = axisSpecification.remapper(absRaw);
			this.axis[name].delta = abs - absOld;	
			this.axis[name].absRaw = absRaw;			
			
		}
	}

	this.update = function()
	{
		//deal with abstracts (buttons and axes) first, using raw
		this.clearAbstractDeltas();
		
		this.updateButtons();
		this.updateAxes();
		
		//TODO fix -- this breaks mouse!
		//then clear raw - necessary or some deltas persist.
		
		
		//NB raws could also be handled simply by maintain two states, old and new, set on key handler, and recalcing deltas between these sets each frame.
		//This would be a more meaningful way, potentially less error-prone approach than resetting at end of frame.
	}
	
	this.finalise = function() //has to be called manually after all app logic has made use of mouse inputs. this is due to the built-in mouse input loop.
	{
		//deal with abstracts (buttons and axes) first, using raw
		this.clearRawDeltas();
	}

	this.initialise = function(domElement) //JS only?
	{
		this.domElement = domElement;

		if ( this.domElement !== document )
		{
			this.domElement.setAttribute( 'tabindex', -1 );
		}
		
		this.domElement.addEventListener( 'contextmenu', function ( event ) { event.preventDefault(); }, false );
		document.addEventListener		( 'mousemove',	bind( this, this.onMouseMove ), false );
		this.domElement.addEventListener( 'mousedown', 	bind( this, this.onMouseDown ), false );
		this.domElement.addEventListener( 'mouseup', 	bind( this, this.onMouseUp ), false );
		this.domElement.addEventListener( 'mousewheel', bind( this, this.onMouseWheel ), false );
		this.domElement.addEventListener( 'keydown', 	bind( this, this.onKeyDown ), false );
		this.domElement.addEventListener( 'keyup', 		bind( this, this.onKeyUp ), false );

		this.handleResize(this);
		
		
		for (var inputName in this.codes)
		{
			var inputCode = this.codes[inputName];
			this.raw[inputCode] = { abs: 0.0, delta: 0.0 };
		}
		
	}



	this.handleResize = function()
	{
		if ( this.domElement === document )
		{
			this.viewportWidthHalf = window.innerWidth / 2;
			this.viewportHeightHalf = window.innerHeight / 2;
		}
		else
		{
			this.viewportWidthHalf = this.domElement.offsetWidth / 2;
			this.viewportHeightHalf = this.domElement.offsetHeight / 2;
		}
	}
	
	this.extractWheelDelta = function(e)
	{
		var d = 0;
		if (e.wheelDelta)   								d = e.wheelDelta;
		if (e.detail)       								d = -e.detail; //FF
		if (e.originalEvent)
		{
			if (e.originalEvent.detail)							d = -e.originalEvent.detail; //FF
			if (e.originalEvent && e.originalEvent.wheelDelta)	d = e.originalEvent.wheelDelta;
		}
		if (d == 0) return 0;
		var sign = d >= 0 ? -1 : 1;

		return sign;
	}
	
	this.onMouseWheel = function(event)
	{
		var input = this;
		var buttonCode = this.codes.MOUSE_WHEEL;
		input.raw[buttonCode].delta = this.extractWheelDelta(event);
	}

	this.onMouseDown = function(event)
	{
		TDP.log('ON MOUSE DOWN', event);
		var input = this; //as bound by ES5's .bind(), you can only specify "this" -- but keep it this way for C compatibility

		if ( input.domElement !== document )
		{
			input.domElement.focus();
			//TDP.log('Focused element is', document.activeElement);
		}

		event.preventDefault();
		event.stopPropagation();

		var eventButtonCode = event.button;
		var buttonName = 'MOUSE_BUTTON_'+this.buttonNamesByEventCode[eventButtonCode];
		buttonCode = this.codes[buttonName];
		input.raw[buttonCode].abs = 1.0;
	}

	this.onMouseUp = function(event)
	{
		//TDP.log('ON MOUSE UP');
		var input = this; //as bound by ES5's .bind(), you can only specify "this" -- but keep it this way for C compatibility

		event.preventDefault();
		event.stopPropagation();

		var eventButtonCode = event.button;
		var buttonName = 'MOUSE_BUTTON_'+this.buttonNamesByEventCode[eventButtonCode];
		buttonCode = this.codes[buttonName];
		input.raw[buttonCode].abs = 0.0;
	}

	this.onMouseMove = function(event)
	{
		var input = this; //as bound by ES5's .bind(), you can only specify "this" -- but keep it this way for C compatibility

		input.raw[this.codes.MOUSE_X].delta = event.clientX - input.raw[this.codes.MOUSE_X].abs;
		input.raw[this.codes.MOUSE_Y].delta = event.clientY - input.raw[this.codes.MOUSE_Y].abs;		
		input.raw[this.codes.MOUSE_X].abs = event.clientX;
		input.raw[this.codes.MOUSE_Y].abs = event.clientY;
		
		input.mouseMovedThisUpdate = true;
	}

	this.onKeyDown = function(event)
	{
		var input = this; //as bound by ES5's .bind(), you can only specify "this" -- but keep it this way for C compatibility
		
		var absOld = input.raw[event.keyCode].abs;
		var abs = input.raw[event.keyCode].abs = 1.0; //maintain all as numbers, for axis application.
		var delta = input.raw[event.keyCode].delta = abs - absOld;
		//TDP.log('!');
	}

	this.onKeyUp = function(event)
	{
		var input = this; //as bound by ES5's .bind(), you can only specify "this" -- but keep it this way for C compatibility

		var absOld = input.raw[event.keyCode].abs;
		var abs = input.raw[event.keyCode].abs = 0.0; //maintain all as numbers, for axis application.
		var delta = input.raw[event.keyCode].delta = abs - absOld;
	}
		
	this.initialise(domElement);
};