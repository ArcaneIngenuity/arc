Disjunction.Core.Updater = function(model) //abstract
{	
	/** Update Model and/or View based on user input, on every frame from start()ed till stop()ed (including the former, but excluding the global update on which the latter occurred). */ 
	this.input = function(app, model, view)
	{
		//abstract
	}
	
	/** Update Model based on other Model values (incl. input), on every frame from start()ed till stop()ed (including the former, but excluding the global update on which the latter occurred). */ 
	this.simulate = function(app, model)
	{
		//abstract
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Loop = Disjunction.Core.Loop;