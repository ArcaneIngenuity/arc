Disjunction.Extensions.Mouse = function()
{
	Device.call(this, 6); //super-constructor (add params after this)
	
	//passive: use as an input event handler - DOM
	this.receive = function(event)
	{
		//console.log('received', event);
		var deviceName = event.type.substring(0, 3);//.substring(2, 5);
		if (deviceName === 'mou')
		{
			var inputType = event.type.substring(5, 7);
			var valueNew, valueOld;
				
			if (inputType === 'do') //down
			{
				var buttonCode = event.button + 2;
				var channel = this.channels[buttonCode];
				//channel.valueLast = channel.value;
				valueOld = channel.value;
				valueNew = channel.value = channel.blocked ? 0 : 1;
				channel.delta = valueNew - valueOld;
			}
			if (inputType === 'up') //up
			{
				var buttonCode = event.button + 2;
				var channel = this.channels[buttonCode];
				//channel.valueLast = channel.value;
				valueOld = channel.value;
				valueNew = channel.value = channel.blocked ? 1 : 0;
				channel.delta = valueNew - valueOld;
				//console.log('channel delta', channel.delta);
			}
			if (inputType === 'mo') //move
			{
				var xChannel = this.channels[disjunction.constants.MOUSE_X];
				var yChannel = this.channels[disjunction.constants.MOUSE_Y];
				
				//xChannel.valueLast = xChannel.value;
				//yChannel.valueLast = yChannel.value;
				//console.log(event);
				//console.log(event.target.getBoundingClientRect());
				if (!xChannel.blocked)
				{
					xChannel.delta += event.clientX - xChannel.value;
					xChannel.value = event.clientX;
				}
				if (!yChannel.blocked)
				{
					yChannel.delta += event.clientY - yChannel.value;
					yChannel.value = event.clientY;
				}
			}
			if (inputType === 'wh') //wheel
			{
				var channel = this.channels[disjunction.constants.MOUSE_WHEEL];
			
				var d = 0;
				if (event.wheelDelta)   								d = event.wheelDelta;
				if (event.detail)       								d = -event.detail; //FF
				if (event.originalEvent)
				{
					if (event.originalEvent.detail)							d = -event.originalEvent.detail; //FF
					if (event.originalEvent && event.originalEvent.wheelDelta)	d = event.originalEvent.wheelDelta;
				}
				if (d == 0) return 0;
				var sign = d >= 0 ? -1 : 1;

				if (!channel.blocked)
				{
					channel.delta = sign;
					channel.value += channel.delta;
				}
			}
		}
		/*
		else if (type === 'con') //context menu
		{
			//TODO
		}
		*/
	}
	
	this.bindToDOM = function(dom)
	{
		dom.addEventListener('mousedown', 	ES5.bind(this, this.receive));
		dom.addEventListener('mouseup',		ES5.bind(this, this.receive));
		dom.addEventListener('mousemove',	ES5.bind(this, this.receive));
		dom.addEventListener('mousewheel',	ES5.bind(this, this.receive));
	}
}

Disjunction.Extensions.Mouse.prototype = new Disjunction.Core.Device();
Disjunction.Extensions.Mouse.prototype.constructor = Disjunction.Extensions.Mouse;

if (disjunction.WINDOW_CLASSES) 
	window.Mouse = Disjunction.Extensions.Mouse;

disjunction.constants.MOUSE_X = 0;
disjunction.constants.MOUSE_Y = 1;
disjunction.constants.MOUSE_BUTTON_LEFT = 2;
disjunction.constants.MOUSE_BUTTON_MIDDLE = 3;
disjunction.constants.MOUSE_BUTTON_RIGHT = 4;
disjunction.constants.MOUSE_WHEEL = 5;