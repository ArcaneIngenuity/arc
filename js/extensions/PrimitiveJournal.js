Disjunction.Extensions.PrimitiveJournal = function(states, deltas)
{
	Disjunction.Core.Journal.call(this, states, deltas);
};

Disjunction.Extensions.PrimitiveJournal.prototype = Object.create(Disjunction.Core.Journal.prototype);
Disjunction.Extensions.PrimitiveJournal.prototype.constructor = Disjunction.Extensions.PrimitiveJournal;

/**
 *	Update the Journal's non-current values.
 *	@const @function
 */
Disjunction.Extensions.PrimitiveJournal.prototype.shift = function()
{
	var length = this.length;
	
	//TODO one loop would work here if the two arrays were known to be of equal length... could this be made an option?
	
	//set all later values by shifting forward
	var states = this.states;
	for (var i = 0; i < states.length - 1; i++)
	{
		this.shiftState(i, i+1);
	}
	
	//set all later values by shifting forward
	var deltas = this.deltas;
	for (var i = 0; i < deltas.length - 1; i++)
	{
		this.shiftDelta(i, i+1);
	}
}


/**
 *	Copy array elements in the appropriate manner for "shifting" the Journal.
 *	@const @function
 *	@param {Number} from The index from which to shift.
 *	@param {Number} to The index to which to shift.
 */
Disjunction.Extensions.PrimitiveJournal.prototype.shiftState = function(from, to)
{
	var states = this.states;
	states[to] = states[from];
}

/**
 *	Copy array elements in the appropriate manner for "shifting" the Journal.
 *	@const @function
 *	@param {Number} from The index from which to shift.
 *	@param {Number} to The index to which to shift.
 */
Disjunction.Extensions.PrimitiveJournal.prototype.shiftDelta = function(from, to)
{
	var deltas = this.deltas;
	deltas[to] = deltas[from];
}

/**
 *	Check whether the Journal has changed between two designated indices.  Override to change the way equality checking is performed.
 *	@abstract @function
 *	@param {Number} from The index from which to shift.
 *	@param {Number} to The index to which to shift.
 */
Disjunction.Extensions.PrimitiveJournal.prototype.changedState = function(from, to)
{
	var states = this.states;
	return states[to] !== states[from];
	
	//TODO or just check delta?
}

if (disjunction.WINDOW_CLASSES) 
	window.PrimitiveJournal = Disjunction.Extensions.PrimitiveJournal;