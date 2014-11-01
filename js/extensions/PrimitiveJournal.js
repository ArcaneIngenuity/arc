Disjunction.Extensions.PrimitiveJournal = function(states, deltas)
{
	Disjunction.Core.Journal.call(this, states, deltas);
};

Disjunction.Extensions.PrimitiveJournal.prototype = Object.create(Disjunction.Core.Journal.prototype);
Disjunction.Extensions.PrimitiveJournal.prototype.constructor = Disjunction.Extensions.PrimitiveJournal;

/**
 *	Copy array elements in the appropriate manner for "shifting" the Journal. Override to change the way copying is performed.
 *	@abstract @function
 *	@param {Number} from The index from which to shift.
 *	@param {Number} to The index to which to shift.
 */
Disjunction.Extensions.PrimitiveJournal.prototype.shiftState = function(from, to)
{
	var states = this.states;
	states[from] = states[to];
}

/**
 *	Copy array elements in the appropriate manner for "shifting" the Journal. Override to change the way copying is performed.
 *	@abstract @function
 *	@param {Number} from The index from which to shift.
 *	@param {Number} to The index to which to shift.
 */
Disjunction.Extensions.PrimitiveJournal.prototype.shiftDelta = function(from, to)
{
	var deltas = this.deltas;
	deltas[from] = deltas[to];
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
	return states[from] !== states[to];
	
	//TODO or just check delta?
}

if (disjunction.WINDOW_CLASSES) 
	window.PrimitiveJournal = Disjunction.Extensions.PrimitiveJournal;