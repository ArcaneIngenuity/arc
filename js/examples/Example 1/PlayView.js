var playView = new View();
playView.start = function()
{
	console.log('start play view');
	var playDOM = document.getElementById('Play');
	playDOM.style.display = 'block';
	
}

playView.update = function()
{
	var ballDOM = document.getElementById('Ball');
	ballDOM.style.left = this.model.ballPos.x + 'px';
	ballDOM.style.top  = this.model.ballPos.y + 'px';
}
	