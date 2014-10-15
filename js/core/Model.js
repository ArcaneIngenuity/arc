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

Disjunction.Core.Model.prototype = Object.create(Disjunction.Core.JournalKeeper.prototype);
Disjunction.Core.Model.prototype.constructor = Disjunction.Core.Model;

if (disjunction.WINDOW_CLASSES) 
	window.Model = Disjunction.Core.Model;
	
	