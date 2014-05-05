var playView = new View();
playView.start = function()
{
	//...PlayView initialisation here... (called when PlayPhase is entered, after PlayCtrl.start)
	console.log('starting PlayView...');
	var playDOM = document.getElementById('Play');
	playDOM.style.display = 'block';
}

playView.update = function()
{
	var ballDOM = document.getElementById('Ball');
	ballDOM.style.left = this.model.ballPos.x + 'px';
	ballDOM.style.top  = this.model.ballPos.y + 'px';
}
	
//Here we haven't overridden PlayView.finish(), so we won't see an "exiting..." log message when we phase back to Menu.