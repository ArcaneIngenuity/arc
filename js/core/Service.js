Disjunction.Core.Service = function()
{
	Disjunction.Core.JournalKeeper.call(this);
	
	this.dispose = function()
	{
		//ABSTRACT
	}
}

Disjunction.Core.Service.prototype = Object.create(Disjunction.Core.JournalKeeper.prototype);
Disjunction.Core.Service.prototype.constructor = Disjunction.Core.Service;

if (disjunction.WINDOW_CLASSES) 
	window.Service = Disjunction.Core.Service;