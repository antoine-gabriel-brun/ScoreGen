#pragma region head

	#pragma once

	using namespace System;

#pragma endregion

namespace ScoreGenLib {
namespace MathForMusic {
namespace Seq { // Suites mathématiques
	
	// 0110 1001 1001 0110 ...
	array<bool, 1>^ thueMorse(int nb) {
			
		array<bool, 1>^ ans = gcnew array<bool, 1>(nb);
			
		ans[0] = false;
		int nbDone = 1;
		int nextIndex;
		
		while (true) {
			for (int i = 0; i < nbDone; i++) {
				nextIndex = nbDone + i;
				if (nextIndex == nb) return ans;
				ans[nextIndex] = !ans[i];
			}
			nbDone += nbDone;	
		}
		
	}
	
	// abc acb abc bac abc acb aca bcb ...
	array<int, 1>^ squareFreeWord3(int nb) {
		
		List<int>^ current = gcnew List<int>;
		current->Add(0);

		// Effectuer les itérations
		while (current->Count < nb) {
			List<int>^ newCurrent = gcnew List<int>;
			for each (int i in current) {
				if (i == 0) {
					newCurrent->Add(0);
					newCurrent->Add(1);
					newCurrent->Add(2);
				} else if (i == 1) {
					newCurrent->Add(0);
					newCurrent->Add(2);
				} else /* (i == 2) */ {
					newCurrent->Add(1);
				}
			}
			current = newCurrent;
		}

		// Convertir en tableau, en gardant exactement le bon nombre d'éléments
		array<int, 1>^ ans = gcnew array<int, 1>(nb);
		int cpt = 0;
		for each (int i in current) {
			ans[cpt++] = i;
			if (cpt >= nb) break;
		}

		// Retourner
		return ans;

	}
	array<int, 1>^ squareFreeWord4(int nb) {
		
		List<int>^ current = gcnew List<int>;
		current->Add(0);
		current->Add(1);
		current->Add(0);
		current->Add(2);
		current->Add(1);
		current->Add(2);

		// Effectuer les itérations
		while (current->Count < nb) {
			List<int>^ newCurrent = gcnew List<int>;
			for each (int i in current) {
				if (i == 0) {
					newCurrent->Add(0);
					newCurrent->Add(3);
					newCurrent->Add(1);
					newCurrent->Add(0);
					newCurrent->Add(2);
					newCurrent->Add(1);
					newCurrent->Add(2);
				} else if (i == 1) {
					newCurrent->Add(0);
					newCurrent->Add(1);
					newCurrent->Add(3);
					newCurrent->Add(0);
					newCurrent->Add(2);
					newCurrent->Add(1);
					newCurrent->Add(2);
				} else if (i == 2) {
					newCurrent->Add(0);
					newCurrent->Add(1);
					newCurrent->Add(0);
					newCurrent->Add(3);
					newCurrent->Add(2);
					newCurrent->Add(1);
					newCurrent->Add(2);
				} else /* (i == 3) */ {
					newCurrent->Add(0);
					newCurrent->Add(1);
					newCurrent->Add(0);
					newCurrent->Add(2);
					newCurrent->Add(3);
					newCurrent->Add(1);
					newCurrent->Add(2);
				}
			}
			current = newCurrent;
		}

		// Convertir en tableau, en gardant exactement le bon nombre d'éléments
		array<int, 1>^ ans = gcnew array<int, 1>(nb);
		int cpt = 0;
		for each (int i in current) {
			ans[cpt++] = i;
			if (cpt >= nb) break;
		}

		// Retourner
		return ans;

	}

	// 0 1 0 2 1 0 3 2 1 0 4 0 3 2 1 0 5 ...
	array<int, 1>^ fractal010210(int nb, bool repeatFirstHalf, double keepRatio, array<int, 1>^ first) {
		
		if (keepRatio < 0 || keepRatio > 1) throw gcnew Exception("The 'keepRatio' argument should be between 0 and 1.");
		if (first == nullptr || first->Length < 1) throw gcnew Exception("The 'first' argument should contain at least 1 element.");

		array<int, 1>^ ans = gcnew array<int, 1>(nb);
		
		int currentLen = Math::Min(first->Length, nb);
		int currentMax = first[0];
		for (int i = 0; i < currentLen; i++) {
			ans[i] = first[i];
			currentMax = Math::Max(currentMax, ans[i]);
		}

		int idealLenAdd, lenAdd, offset;

		while (currentLen < nb) {
			ans[currentLen] = ++currentMax;
			idealLenAdd = Math::Max(1, (int)Math::Round(keepRatio * currentLen));
			lenAdd = Math::Min(nb - currentLen - 1, idealLenAdd);
			offset = repeatFirstHalf ? 0 : currentLen - idealLenAdd;
			for (int i = 0; i < lenAdd; i++) ans[currentLen + 1 + i] = ans[i + offset];
			currentLen += 1 + lenAdd;
		}

		return ans;

	}
	array<int, 1>^ fractal010210(int nb) {

		return fractal010210(nb, false, 0.5, gcnew array<int, 1>{ 0 });

	}

}
}
}
