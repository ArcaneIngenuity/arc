Mouse = function()
{
	InputDevice.call(this, 6); //super-constructor (add params after this)
	
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
				valueNew = channel.value = 1;
				channel.delta = valueNew - valueOld;
			}
			if (inputType === 'up') //up
			{
				var buttonCode = event.button + 2;
				var channel = this.channels[buttonCode];
				//channel.valueLast = channel.value;
				valueOld = channel.value;
				valueNew = channel.value = 0;
				channel.delta = valueNew - valueOld;
				console.log('channel delta', channel.delta);
			}
			if (inputType === 'mo') //move
			{
				var xChannel = this.channels[MOUSE_X];
				var yChannel = this.channels[MOUSE_Y];
				
				//xChannel.valueLast = xChannel.value;
				//yChannel.valueLast = yChannel.value;
				//console.log(event);
				//console.log(event.target.getBoundingClientRect());
				xChannel.delta += event.clientX - xChannel.value;
				xChannel.value = event.clientX;
				yChannel.delta += event.clientY - yChannel.value;
				yChannel.value = event.clientY;
			}
			if (inputType === 'wh') //wheel
			{
				//TODO
			}
		}
		/*
		else if (type === 'con') //context menu
		{
			//TODO
		}
		*/
	}
}

Mouse.prototype = new InputDevice();
Mouse.prototype.constructor = Mouse;

MOUSE_X = 0;
MOUSE_Y = 1;
MOUSE_BUTTON_LEFT = 2;
MOUSE_BUTTON_MIDDLE = 3;
MOUSE_BUTTON_RIGHT = 4;
MOUSE_WHEEL = 5;