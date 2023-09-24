#pragma region head

	#pragma once

	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;

#pragma endregion

namespace ScoreGenLib {
namespace MathForMusic {
	
	// L-système
	public ref class LSystem {
		
		// Champs
		private:
			int _nb;
			array<array<int, 1>^, 1>^ _rules;

		// Constructeur
		public:
			LSystem(int nb) :
				_nb(nb),
				_rules(gcnew array<array<int, 1>^, 1>(nb))
			{ }

		// Ajout de règles
		public:
			void setRule(int initial, array<int, 1>^ final) {
				for each (int i in final) {
					if (i < 0 || i >= _nb) throw gcnew Exception("All elements should be between 0 and " + (_nb - 1) + ".");
				}
				_rules[initial] = copy(final);
			}

		// Utilisation du L-système
		public:
			array<int, 1>^ generate(int initialValue, int nbIterations) {
				return generate(gcnew array<int, 1>{initialValue}, nbIterations);
			}
			array<int, 1>^ generate(array<int, 1>^ initialSeq, int nbIterations) {
				if (nbIterations < 0) throw gcnew Exception("The number of iterations should be positive.");
				return generate_rec(gcnew List<int>(initialSeq), nbIterations)->ToArray();
			}
		private:
			List<int>^ generate_rec(List<int>^ initialSeq, int nbIterations) {
				if (nbIterations == 0) {
					return initialSeq;
				} else {
					List<int>^ ans = gcnew List<int>;
					for each (int i in initialSeq) {
						array<int, 1>^ rule = _rules[i];
						if (rule == nullptr) {
							ans->Add(i);
						} else {
							for each (int j in rule) {
								ans->Add(j);
							}
						}
					}
					return generate_rec(ans, nbIterations - 1);
				}
			}

	};

}
}
