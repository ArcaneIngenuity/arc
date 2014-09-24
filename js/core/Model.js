Disjunction.Core.Model = function()
{
	Disjunction.Core.JournalKeeper.call(this); 
	
	this.serialise = this.serialize = function()
	{
		//ABSTRACT
	}
	
	this.deserialise = this.deserialize = function(serialised)
	{
		//ABSTRACT
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Model = Disjunction.Core.Model;
	
	