#pragma region head

	#pragma once

	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;

	#include "2. Note, Rest, Tuplet.h"

#pragma endregion

namespace ScoreGenLib {
namespace Music {

	ref class Block;
	ref class Score;
	ref struct ScoreAndInfos;

	// Localisation temporelle dans un block
	public ref class BarIndexAndTime {
			
		// Champs
		private:
			int _bar;
			int _timeInBar_1024th;
		private:
			static Exception^ exc_positiveTime = gcnew Exception("A temporal point should be positive.");
			static Exception^ exc_nonNegativeBar = gcnew Exception("Group indices are 1 based.");

		// Accesseurs
		public:
			property int bar {
				int get() {
					return _bar;
				}
			}
			property int timeInBar_1024th {
				int get() {
					return _timeInBar_1024th;
				}
			}

		// Constructeurs
		public:
			BarIndexAndTime() :
				_bar(1),
				_timeInBar_1024th(0)
			{ }
			BarIndexAndTime(int bar) :
				_bar(bar),
				_timeInBar_1024th(0)
			{
				
				if (bar < 1) throw exc_nonNegativeBar;
				
			}
			BarIndexAndTime(int bar, int timeInBar_1024th) :
				_bar(bar),
				_timeInBar_1024th(timeInBar_1024th)
			{
				
				if (bar < 1) throw exc_nonNegativeBar;
				else if (timeInBar_1024th < 0) throw exc_positiveTime;
				
			}
			BarIndexAndTime(int bar, RhythmFigure^ afterThisDuration) :
				_bar(bar),
				_timeInBar_1024th(afterThisDuration->duration_1024th)
			{
				
				if (bar < 1) throw exc_nonNegativeBar;
				
			}
			BarIndexAndTime(int bar, RhythmFigure^ afterThisDuration, int afterThisDuration_mult) :
				_bar(bar),
				_timeInBar_1024th(afterThisDuration->duration_1024th * afterThisDuration_mult)
			{
				
				if (bar < 1) throw exc_nonNegativeBar;
				else if (afterThisDuration_mult < 0) throw exc_positiveTime;
				
			}
		public:
			static BarIndexAndTime^ getInBlock(Block^ block, int time_1024th);
			static BarIndexAndTime^ getInBlock(Block^ block, RhythmFigure^ afterThisDuration) {
					
				return getInBlock(block, afterThisDuration->duration_1024th);

			}
			static BarIndexAndTime^ getInBlock(Block^ block, RhythmFigure^ afterThisDuration, int afterThisDuration_mult) {
					
				return getInBlock(block, afterThisDuration->duration_1024th * afterThisDuration_mult);

			}

	};
	
	// Options pour l'écriture d'un bloc dans un autre
	public ref class TimeSignaturesOverwriteBehaviour {
		
		// Variables
		private:
			int val;
			
		// Constructeur
		private:
			TimeSignaturesOverwriteBehaviour(int val) :
				val(val)
			{ }
		
		// Valeurs
		public:
			static TimeSignaturesOverwriteBehaviour^ requestSameSignatures    = gcnew TimeSignaturesOverwriteBehaviour(1);
			static TimeSignaturesOverwriteBehaviour^ adaptToCurrentSignatures = gcnew TimeSignaturesOverwriteBehaviour(2);
		public:
			static TimeSignaturesOverwriteBehaviour^ byDefault = adaptToCurrentSignatures;
			
		// Opérateurs
		public:
			static inline bool operator ==  (TimeSignaturesOverwriteBehaviour^ a, TimeSignaturesOverwriteBehaviour^ b) {
		
				return a->val == b->val;
		
			}
			static inline bool operator !=  (TimeSignaturesOverwriteBehaviour^ a, TimeSignaturesOverwriteBehaviour^ b) {
					
				return a->val != b->val;
		
			}
			static inline      operator int (TimeSignaturesOverwriteBehaviour^ a) {
		
				return a->val;
		
			}
		
	};
	public ref class NotesOverwriteBehaviour {
		
		// Variables
		private:
			int val;
			
		// Constructeur
		private:
			NotesOverwriteBehaviour(int val) :
				val(val)
			{ }
		
		// Valeurs
		public:
			static NotesOverwriteBehaviour^ requestNoCollisions = gcnew NotesOverwriteBehaviour(1);
			static NotesOverwriteBehaviour^ cutIfNecessary      = gcnew NotesOverwriteBehaviour(2);
		public:
			static NotesOverwriteBehaviour^ byDefault = requestNoCollisions;
			
		// Opérateurs
		public:
			static inline bool operator ==  (NotesOverwriteBehaviour^ a, NotesOverwriteBehaviour^ b) {
		
				return a->val == b->val;
		
			}
			static inline bool operator !=  (NotesOverwriteBehaviour^ a, NotesOverwriteBehaviour^ b) {
					
				return a->val != b->val;
		
			}
			static inline      operator int (NotesOverwriteBehaviour^ a) {
		
				return a->val;
		
			}
		
	};
	public ref class TooMuchPartsBehaviour {
		
		// Variables
		private:
			int val;
			
		// Constructeur
		private:
			TooMuchPartsBehaviour(int val) :
				val(val)
			{ }
		
		// Valeurs
		public:
			static TooMuchPartsBehaviour^ requestEnoughParts     = gcnew TooMuchPartsBehaviour(1);
			static TooMuchPartsBehaviour^ addOnlyOnExistingParts = gcnew TooMuchPartsBehaviour(2);
			static TooMuchPartsBehaviour^ addLackingParts        = gcnew TooMuchPartsBehaviour(3);
		public:
			static TooMuchPartsBehaviour^ byDefault = addOnlyOnExistingParts;
			
		// Opérateurs
		public:
			static inline bool operator ==  (TooMuchPartsBehaviour^ a, TooMuchPartsBehaviour^ b) {
		
				return a->val == b->val;
		
			}
			static inline bool operator !=  (TooMuchPartsBehaviour^ a, TooMuchPartsBehaviour^ b) {
					
				return a->val != b->val;
		
			}
			static inline      operator int (TooMuchPartsBehaviour^ a) {
		
				return a->val;
		
			}
		
	};
	public ref class TooMuchBarsBehaviour {
		
		// Variables
		private:
			int val;
			
		// Constructeur
		private:
			TooMuchBarsBehaviour(int val) :
				val(val)
			{ }
		
		// Valeurs
		public:
			static TooMuchBarsBehaviour^ requestEnoughBars                    = gcnew TooMuchBarsBehaviour(1);
			static TooMuchBarsBehaviour^ addOnlyOnExistingBars                = gcnew TooMuchBarsBehaviour(2);
			static TooMuchBarsBehaviour^ addOneBigBarAtTheEnd                 = gcnew TooMuchBarsBehaviour(3);
			static TooMuchBarsBehaviour^ addCommonBarsAtTheEnd                = gcnew TooMuchBarsBehaviour(4);
			static TooMuchBarsBehaviour^ addBarsAtTheEndWithLastTimeSignature = gcnew TooMuchBarsBehaviour(5);
		public:
			static TooMuchBarsBehaviour^ byDefault = addCommonBarsAtTheEnd;
			
		// Opérateurs
		public:
			static inline bool operator ==  (TooMuchBarsBehaviour^ a, TooMuchBarsBehaviour^ b) {
		
				return a->val == b->val;
		
			}
			static inline bool operator !=  (TooMuchBarsBehaviour^ a, TooMuchBarsBehaviour^ b) {
					
				return a->val != b->val;
		
			}
			static inline      operator int (TooMuchBarsBehaviour^ a) {
		
				return a->val;
		
			}
		
	};
	public ref struct WriteOptions {

		// Champs
		public:
			TimeSignaturesOverwriteBehaviour^ timeSignaturesOverwriteBehaviour;
			NotesOverwriteBehaviour^          notesOverwriteBehaviour         ;
			TooMuchPartsBehaviour^            tooMuchPartsBehaviour           ;
			TooMuchBarsBehaviour^             tooMuchBarsBehaviour            ;

		// Constructeurs
		public:
			WriteOptions(TimeSignaturesOverwriteBehaviour^ a, NotesOverwriteBehaviour^ b, TooMuchPartsBehaviour^ c, TooMuchBarsBehaviour^ d) :
				timeSignaturesOverwriteBehaviour(a),
				notesOverwriteBehaviour         (b),
				tooMuchPartsBehaviour           (c),
				tooMuchBarsBehaviour            (d)
			{ }
			WriteOptions(TimeSignaturesOverwriteBehaviour^ a, NotesOverwriteBehaviour^ b, TooMuchPartsBehaviour^ c                         ) :
				timeSignaturesOverwriteBehaviour(a),
				notesOverwriteBehaviour         (b),
				tooMuchPartsBehaviour           (c),
				tooMuchBarsBehaviour            (TooMuchBarsBehaviour::byDefault)
			{ }
			WriteOptions(TimeSignaturesOverwriteBehaviour^ a, NotesOverwriteBehaviour^ b                          , TooMuchBarsBehaviour^ d) :
				timeSignaturesOverwriteBehaviour(a),
				notesOverwriteBehaviour         (b),
				tooMuchPartsBehaviour           (TooMuchPartsBehaviour::byDefault),
				tooMuchBarsBehaviour            (d)
			{ }
			WriteOptions(TimeSignaturesOverwriteBehaviour^ a, NotesOverwriteBehaviour^ b                                                   ) :
				timeSignaturesOverwriteBehaviour(a),
				notesOverwriteBehaviour         (b),
				tooMuchPartsBehaviour           (TooMuchPartsBehaviour::byDefault),
				tooMuchBarsBehaviour            (TooMuchBarsBehaviour::byDefault)
			{ }
			WriteOptions(TimeSignaturesOverwriteBehaviour^ a                            , TooMuchPartsBehaviour^ c, TooMuchBarsBehaviour^ d) :
				timeSignaturesOverwriteBehaviour(a),
				notesOverwriteBehaviour         (NotesOverwriteBehaviour::byDefault),
				tooMuchPartsBehaviour           (c),
				tooMuchBarsBehaviour            (d)
			{ }
			WriteOptions(TimeSignaturesOverwriteBehaviour^ a                            , TooMuchPartsBehaviour^ c                         ) :
				timeSignaturesOverwriteBehaviour(a),
				notesOverwriteBehaviour         (NotesOverwriteBehaviour::byDefault),
				tooMuchPartsBehaviour           (c),
				tooMuchBarsBehaviour            (TooMuchBarsBehaviour::byDefault)
			{ }
			WriteOptions(TimeSignaturesOverwriteBehaviour^ a                                                      , TooMuchBarsBehaviour^ d) :
				timeSignaturesOverwriteBehaviour(a),
				notesOverwriteBehaviour         (NotesOverwriteBehaviour::byDefault),
				tooMuchPartsBehaviour           (TooMuchPartsBehaviour::byDefault),
				tooMuchBarsBehaviour            (d)
			{ }
			WriteOptions(TimeSignaturesOverwriteBehaviour^ a                                                                               ) :
				timeSignaturesOverwriteBehaviour(a),
				notesOverwriteBehaviour         (NotesOverwriteBehaviour::byDefault),
				tooMuchPartsBehaviour           (TooMuchPartsBehaviour::byDefault),
				tooMuchBarsBehaviour            (TooMuchBarsBehaviour::byDefault)
			{ }
			WriteOptions(                                     NotesOverwriteBehaviour^ b, TooMuchPartsBehaviour^ c, TooMuchBarsBehaviour^ d) :
				timeSignaturesOverwriteBehaviour(TimeSignaturesOverwriteBehaviour::byDefault),
				notesOverwriteBehaviour         (b),
				tooMuchPartsBehaviour           (c),
				tooMuchBarsBehaviour            (d)
			{ }
			WriteOptions(                                     NotesOverwriteBehaviour^ b, TooMuchPartsBehaviour^ c                         ) :
				timeSignaturesOverwriteBehaviour(TimeSignaturesOverwriteBehaviour::byDefault),
				notesOverwriteBehaviour         (b),
				tooMuchPartsBehaviour           (c),
				tooMuchBarsBehaviour            (TooMuchBarsBehaviour::byDefault)
			{ }
			WriteOptions(                                     NotesOverwriteBehaviour^ b                          , TooMuchBarsBehaviour^ d) :
				timeSignaturesOverwriteBehaviour(TimeSignaturesOverwriteBehaviour::byDefault),
				notesOverwriteBehaviour         (b),
				tooMuchPartsBehaviour           (TooMuchPartsBehaviour::byDefault),
				tooMuchBarsBehaviour            (d)
			{ }
			WriteOptions(                                     NotesOverwriteBehaviour^ b                                                   ) :
				timeSignaturesOverwriteBehaviour(TimeSignaturesOverwriteBehaviour::byDefault),
				notesOverwriteBehaviour         (b),
				tooMuchPartsBehaviour           (TooMuchPartsBehaviour::byDefault),
				tooMuchBarsBehaviour            (TooMuchBarsBehaviour::byDefault)
			{ }
			WriteOptions(                                                                 TooMuchPartsBehaviour^ c, TooMuchBarsBehaviour^ d) :
				timeSignaturesOverwriteBehaviour(TimeSignaturesOverwriteBehaviour::byDefault),
				notesOverwriteBehaviour         (NotesOverwriteBehaviour::byDefault),
				tooMuchPartsBehaviour           (c),
				tooMuchBarsBehaviour            (d)
			{ }
			WriteOptions(                                                                 TooMuchPartsBehaviour^ c                         ) :
				timeSignaturesOverwriteBehaviour(TimeSignaturesOverwriteBehaviour::byDefault),
				notesOverwriteBehaviour         (NotesOverwriteBehaviour::byDefault),
				tooMuchPartsBehaviour           (c),
				tooMuchBarsBehaviour            (TooMuchBarsBehaviour::byDefault)
			{ }
			WriteOptions(                                                                                           TooMuchBarsBehaviour^ d) :
				timeSignaturesOverwriteBehaviour(TimeSignaturesOverwriteBehaviour::byDefault),
				notesOverwriteBehaviour         (NotesOverwriteBehaviour::byDefault),
				tooMuchPartsBehaviour           (TooMuchPartsBehaviour::byDefault),
				tooMuchBarsBehaviour            (d)
			{ }
			WriteOptions(                                                                                                                  ) :
				timeSignaturesOverwriteBehaviour(TimeSignaturesOverwriteBehaviour::byDefault),
				notesOverwriteBehaviour         (NotesOverwriteBehaviour::byDefault),
				tooMuchPartsBehaviour           (TooMuchPartsBehaviour::byDefault),
				tooMuchBarsBehaviour            (TooMuchBarsBehaviour::byDefault)
			{ }
		
		// Valeurs
		public:
			static WriteOptions^ allStrict() {
				
				return gcnew WriteOptions(
					TimeSignaturesOverwriteBehaviour::requestSameSignatures,
					NotesOverwriteBehaviour         ::requestNoCollisions  ,
					TooMuchPartsBehaviour           ::requestEnoughParts   ,
					TooMuchBarsBehaviour            ::requestEnoughBars
				);
			
			}
			static WriteOptions^ allPermissive() {
				
				return gcnew WriteOptions(
					TimeSignaturesOverwriteBehaviour::adaptToCurrentSignatures            ,
					NotesOverwriteBehaviour         ::cutIfNecessary                      ,
					TooMuchPartsBehaviour           ::addLackingParts                     ,
					TooMuchBarsBehaviour            ::addBarsAtTheEndWithLastTimeSignature
				);
			
			}
			static WriteOptions^ allDefault() {
			
				return gcnew WriteOptions(
					TimeSignaturesOverwriteBehaviour::byDefault,
					NotesOverwriteBehaviour         ::byDefault,
					TooMuchPartsBehaviour           ::byDefault,
					TooMuchBarsBehaviour            ::byDefault
				);
			
			}

	};
	
}
}
