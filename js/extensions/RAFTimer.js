disjunction.extensions.RAFTimer = function()
{
	//check whether RAF is supported in user agent.
	if ( !window.requestAnimationFrame )
		if (!(
			window.webkitRequestAnimationFrame ||
			window.mozRequestAnimationFrame ||
			window.oRequestAnimationFrame ||
			window.msRequestAnimationFrame))
			throw 'Error: Browser does not support RequestAnimationFrame (RAF). Use Timer instead.';
			
	Timer.call(this); //super-constructor (params after this)
	
	this.start = function()
	{
		timingID = requestAnimationFrame( ES5.bind(this, this.start) ); //request timer update at next vsync
		
		this.update();
		
		//In C: running = true; while(running) this.update()
	}

	this.stop = function()
	{
		if (timingID)
			cancelAnimationFrame(requestAnimationFrameID);
			
		//In C: running = false;
	}
}

disjunction.extensions.RAFTimer.prototype = Object.create(disjunction.core.Timer.prototype);
disjunction.extensions.RAFTimer.prototype.constructor = disjunction.extensions.RAFTimer;