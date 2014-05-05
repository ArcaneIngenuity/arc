var playCtrl = new Ctrl();
playCtrl.start = function()
{
	//Gameplay phase initialisation logic here...
	
	console.log('start play ctrl');
	this.model.ballPos.x = 100;
	this.model.ballPos.y = 100;
	this.model.ballPos.xVelocity = Math.random() * 30;
	this.model.ballPos.yVelocity = Math.random() * 30;
};

playCtrl.finish = function()
{	
	//Gameplay phase cleanup logic here...
	
	var playDOM = document.getElementById('Play');
	playDOM.style.display = 'none';
}

playCtrl.update = function(deltaSec)
{
	//Gameplay phase update logic here...

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