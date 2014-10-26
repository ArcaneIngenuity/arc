Disjunction.Core.JournalKeeper = function()	
{
	this.journals = []; //array of arrays TODO array of Journal (inheriting from Array for faster internal access) to support offsets into an existing, large (typed) array

	this.regulateJournals = function()
	{
		var journals = this.journals;
		var length = journals.length;
		for (var i = 0; i < length; i++)
		{
			var journal = journals[i];
			journal.copy();
			journal.unlock();
		}
	}
}