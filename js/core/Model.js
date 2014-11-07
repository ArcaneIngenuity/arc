Disjunction.Core.Model = function()
{
	this.journals = []; //array of arrays TODO array of Journal (inheriting from Array for faster internal access) to support offsets into an existing, large (typed) array

	this.progressJournals = function()
	{
		var journals = this.journals;
		var length = journals.length;
		for (var i = 0; i < length; i++)
		{
			var journal = journals[i];
			
			//journal.shiftStates();
			//journal.shiftDeltas();
			journal.shift(); //allows combining of state and delta shift loops for fewer conditionals
			//journal.clear();
			journal.unlock();
		}
	}
	
	//Abstract methods
	
	this.serialise = this.serialize = function()
	{
	}
	
	this.deserialise = this.deserialize = function(serialised)
	{
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Model = Disjunction.Core.Model;
	
	