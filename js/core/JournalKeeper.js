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
			
			//NB strictly there should only be regulated journals herein. TODO make an addJournal method which only adds journals with regulate==true to this keeper's list; use this in Builder.
			if (!journal.locked) //give kept journal a tick if it hasn't had one done in user code
				journal.postUpdate();
				
			journal.unlock();
		}
	}
}