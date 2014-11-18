Disjunction.Extensions.Strategiser = function(numChannels)
{
	var channels = new Array(numChannels);
	this.candidatesByChannel = {};
	var justStopped = [];
	
	this.run = function()
	{
		var strategy;
		justStopped.splice(0);
		
		for (var i = 0; i < channels.length; i++)
		{
			strategy = channels[i];
			if (strategy) //if no strategy running on this channel
			{
				//strategy.update(); //includes run on the final update before stop.
				if (strategy.isToStop())
				{
					strategy.stop();
					justStopped.push(strategy); //to prevent immediate restart of strategy in same update
					channels[i] = undefined;
					strategy = undefined; //to prevent update below
				}
			}
			
			if (!strategy) //else not sufficient: if within a category, a potential new strategy's start condition is the same as a just-stopped strategy's stop condition, the start condition fires next frame which is too late when using the ever-changing Journal in said condition.
			{
				var candidates = this.candidatesByChannel[i];
				for (var c = 0; c < candidates.length; c++)
				{	
					var strategy = candidates[c];
					if (justStopped.indexOf(strategy) == -1) //we cannot immediately restart a strategy that has just stopped -- wait till after journals have been updated
					{
						if (strategy.isToStart())
						{
							channels[i] = strategy; //channel may run only one within that category
							strategy.start();
						}
						else
							strategy = undefined; //to prevent update below
					}
				}
			}
			
			if (strategy)
				strategy.update();
		}
	}
}

if (disjunction.WINDOW_CLASSES) 
	window.Strategiser = Disjunction.Extensions.Strategiser;