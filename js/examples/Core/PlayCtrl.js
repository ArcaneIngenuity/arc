var playCtrl = new Ctrl();
playCtrl.start = function()
{
	//...PlayCtrl initialisation here... (called when PlayPhase is entered, before PlayView.start)
	console.log('starting PlayCtrl...');
	
	this.model.ballPos.x = 100;
	this.model.ballPos.y = 100;
	this.model.ballPos.xVelocity = Math.random() * 30;
	this.model.ballPos.yVelocity = Math.random() * 30;
};

playCtrl.finish = function()
{	
	//...PlayCtrl cleanup here... (called when PlayPhase is entered)
	console.log('finishing PlayCtrl...');
	
	var playDOM = document.getElementById('Play');
	playDOM.style.display = 'none';
}

playCtrl.update = function(deltaSec)
{
	//...PlayCtrl update here... (called when PlayPhase is entered)

	//UI
	if (app.model.hitOption)
	{
		//console.log('?')
		app.phaser.change(app.model.hitOption);
		app.model.hitOption = undefined; //reset the value
	}

	//simulation
	this.model.ballPos.x += this.model.ballPos.xVelocity * deltaSec; //1px per sec
	this.model.ballPos.y += this.model.ballPos.yVelocity * deltaSec; //1px per sec
};