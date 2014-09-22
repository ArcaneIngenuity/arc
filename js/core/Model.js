//all Bindable stuff for Model -- need not be extended, can be accessed as "static" in JS, i.e. from this object
disjunction.core.Model = function()
{
	this.journals = {}; //array of arrays TODO array of Journal (inheriting from Array for faster internal access) to support offsets into an existing large, fast typed array
	
	this.addJournal = function(id, length) //we pass in the array rather than creating it here since this allows us to use subarrays within an existing large array
	{
		this.journals[id] = new Array(length);
	}
	
	this.updateJournals = function()
	{
		var journals = this.journals;
		for (var j in journals)
		{
			var journal = this.journals[j];
			var entriesLength = journal.length;
			for (var e = 0; e < entriesLength - 1; e++)
			{
				journal[e+1] = journal[e];
			}
		}
	}
	
	this.changed = function(id, indexB, indexA) //make new entry last and optional
	{
		indexA = indexA || 0;
		indexB = indexB || 1;
		var journal = this.journals[id];
		return journal[indexA] != journal[indexB];
	}

	this.serialise = this.serialize = function()
	{
		//ABSTRACT
	}
	
	this.deserialise = this.deserialize = function(serialised)
	{
		//ABSTRACT
	}
};