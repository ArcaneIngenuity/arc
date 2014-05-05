var TimerRAF = function(app, isUpdatingView, isUpdatingCtrl)
{
	//first check if RAF is supported in user agent.
	
	if ( !window.requestAnimationFrame )
		if (!(
			window.webkitRequestAnimationFrame ||
			window.mozRequestAnimationFrame ||
			window.oRequestAnimationFrame ||
			window.msRequestAnimationFrame))
			throw 'Error: Browser does not support RequestAnimationFrame (RAF). Use Timer instead.';
			
	//else proceed smoothly.
	Timer.call(this, app, undefined, isUpdatingView, isUpdatingCtrl);
	
	this.start = function()
	{
		//console.log(this)
		timingID = requestAnimationFrame( Utility.bind(this, this.start) ); //request timer update at next vsync
		
		this.update();
	}

	this.stop = function()
	{
		if (timingID)
			cancelAnimationFrame(requestAnimationFrameID);
	}
}

//https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/create
TimerRAF.prototype = Object.create(Timer.prototype);
TimerRAF.prototype.constructor = TimerRAF;

//console.log(new TimerRAF(0.1, 'blah', true, false));