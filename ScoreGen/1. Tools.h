#pragma region head

	#pragma once
	
	using namespace System;

#pragma endregion

namespace ScoreGenLib {
namespace Music {

	// Fraction générique non mutable, stockée sous forme unique :
	//   - Le dénominateur est stocké strictement positif : seul le numérateur porte le signe
	//   - La fraction est stockée simplifiée : pgcd(numérateur, dénominateur) = 1
	//   - La fraction nulle est toujours stockée comme 0 / 1
	template <typename INT> public ref class GenericFraction {
	
		// Champs
		private:
			INT _a;
			INT _b;

		// Accesseurs
		public:
			property INT a {
				inline INT get() {
					return _a;
				}
			}
			property INT b {
				inline INT get() {
					return _b;
				}
			}

		// Constructeurs
		public:
			inline GenericFraction(INT a, INT b) :
				_a(a),
				_b(b)
			{
					
				// Incorrect ?
				if (_b == 0) throw gcnew Exception("The denominator shouldn't be zero.");

				// Simplifier le signe
				if (_a == 0) {
					b = 1;
				} else if (_b < 0) {
					_a = -_a;
					_b = -_b;
				}

				// Calculer le PGCD et simplifier
				int div = gcd(_a, _b);
				_a /= div;
				_b /= div;
				
			}
			inline GenericFraction(INT a) :
				_a(a),
				_b(1)
			{ }
			
		// Opérateurs
		public:
			// Egalité
			static inline bool operator == (GenericFraction^ f, GenericFraction^ g) { return f->a == g->a && f->b == g->b; }
			static inline bool operator != (GenericFraction^ f, GenericFraction^ g) { return f->a != g->a || f->b != g->b; }
			static inline bool operator == (GenericFraction^ f,              INT g) { return f->a == g    && f->b == 1; }
			static inline bool operator != (GenericFraction^ f,              INT g) { return f->a != g    || f->b != 1; }
			static inline bool operator == (INT              f, GenericFraction^ g) { return g == f; }
			static inline bool operator != (INT              f, GenericFraction^ g) { return g != f; }
		public:
			// Comparaison
			static inline bool operator >  (GenericFraction^ f, GenericFraction^ g) { return f->a * g->b >  f->b * g->a; }
			static inline bool operator >= (GenericFraction^ f, GenericFraction^ g) { return f->a * g->b >= f->b * g->a; }
			static inline bool operator <  (GenericFraction^ f, GenericFraction^ g) { return f->a * g->b <  f->b * g->a; }
			static inline bool operator <= (GenericFraction^ f, GenericFraction^ g) { return f->a * g->b <= f->b * g->a; }
			static inline bool operator >  (GenericFraction^ f, INT              g) { return f->a        >  f->b * g   ; }
			static inline bool operator >= (GenericFraction^ f, INT              g) { return f->a        >= f->b * g   ; }
			static inline bool operator <  (GenericFraction^ f, INT              g) { return f->a        <  f->b * g   ; }
			static inline bool operator <= (GenericFraction^ f, INT              g) { return f->a        <= f->b * g   ; }
			static inline bool operator >  (INT              f, GenericFraction^ g) { return f    * g->b >  g->a       ; }
			static inline bool operator >= (INT              f, GenericFraction^ g) { return f    * g->b >= g->a       ; }
			static inline bool operator <  (INT              f, GenericFraction^ g) { return f    * g->b <  g->a       ; }
			static inline bool operator <= (INT              f, GenericFraction^ g) { return f    * g->b <= g->a       ; }
		public:
			// Arithmétique
			static inline GenericFraction^ operator + (GenericFraction^ f) { return gcnew GenericFraction( f->a, f->b); }
			static inline GenericFraction^ operator - (GenericFraction^ f) { return gcnew GenericFraction(-f->a, f->b); }
			static inline GenericFraction^ operator + (GenericFraction^ f, GenericFraction^ g) {
					
				INT new_b = lcm(f->b, g->b);

				INT mult_f = new_b / f->b;
				INT mult_g = new_b / g->b;

				INT new_a = f->a * mult_f + g->a * mult_g;

				return gcnew GenericFraction(new_a, new_b);

			}
			static inline GenericFraction^ operator + (GenericFraction^ f, INT              g) {
				
				return f + gcnew GenericFraction(g);

			}
			static inline GenericFraction^ operator + (INT              f, GenericFraction^ g) {
				
				return gcnew GenericFraction(f) + g;

			}
			static inline GenericFraction^ operator - (GenericFraction^ f, GenericFraction^ g) {
				
				return f + (-g);

			}
			static inline GenericFraction^ operator - (GenericFraction^ f, INT              g) {
				
				return f + (-g);

			}
			static inline GenericFraction^ operator - (INT              f, GenericFraction^ g) {
				
				return f + (-g);

			}
			static inline GenericFraction^ operator * (GenericFraction^ f, INT              g) {
					
				return gcnew GenericFraction(f->a * g, f->b);

			}
			static inline GenericFraction^ operator * (INT              f, GenericFraction^ g) {
					
				return g * f;

			}
			static inline GenericFraction^ operator * (GenericFraction^ f, GenericFraction^ g) {
					
				return gcnew GenericFraction(f->a * g->a, f->b * g->b);

			}
			static inline GenericFraction^ operator / (GenericFraction^ f, INT              g) {
				
				return gcnew GenericFraction(f->a, f->b * g);

			}
			static inline GenericFraction^ operator / (INT              f, GenericFraction^ g) {
					
				return gcnew GenericFraction(f * g->b, g->a);

			}
			static inline GenericFraction^ operator / (GenericFraction^ f, GenericFraction^ g) {
				
				return gcnew GenericFraction(f->a * g->b, f->b * g->a);

			}

		// Fonctions
		public:
			inline bool isInteger() {
				
				return b == 1;

			}
		public:
			static int gcd(INT a, INT b) {
					
				a = Math::Abs(a);
				b = Math::Abs(b);
				INT temp;

				while (b > 0) {
					temp = b;
					b = a % b;
					a = temp;
				}

				return a;

			}
			static int lcm(INT a, INT b) {
					
				return Math::Abs((a / gcd(a, b)) * b);

			}

	};
	typedef GenericFraction<int> Fraction; namespace { /* Fraction commune, basée sur des entiers de type 'int'. */ };

	// Générateur aléatoire
	public ref class Rand abstract {
	
		// Champs
		private:
			static Random^ r = gcnew Random;
	
		// Fonctions
		public:
			static void randomize() {
				
				r = gcnew Random();
	
			}
			static void randomize(int seed) {
				
				r = gcnew Random(seed);
	
			}
		public:
			static inline double double01() {
				
				return r->NextDouble();
	
			}
			static inline double double_m1_p1() {
				
				return r->NextDouble() * 2.0 - 1.0;
	
			}
			static inline int integer(int min, int exclusiveMax) {
				
				return r->Next(min, exclusiveMax);
	
			}
			static inline int integer(int exclusiveMax) {
				
				return r->Next(exclusiveMax);
	
			}
		public:
			template <typename T> static inline T chooseIn(array<T, 1>^ elements) {
				
				return elements[r->Next(0, elements->Length)];
			
			}
			template <typename T> static inline T chooseIn(array<T, 1>^ elements, int min, int exclusiveMax) {
				
				return elements[r->Next(min, exclusiveMax)];
			
			}
			template <typename T> static inline T chooseIn(List<T>^ elements) {
				
				return elements[r->Next(0, elements->Count)];
			
			}
			template <typename T> static inline T chooseIn(List<T>^ elements, int min, int exclusiveMax) {
				
				return elements[r->Next(min, exclusiveMax)];
			
			}
		public:
			template <typename T> static void shuffle(array<T, 1>^ elements) {
				
				int len = elements->Length;

				for (int i = 0; i < len - 1; i++) {
					int j = Rand::integer(i, len);
					if (i != j) {
						T _exchange = elements[i];
						elements[i] = elements[j];
						elements[j] = _exchange;
					}
				}

			}
	
	};

	// Tableaux et listes
	template <typename T> array<T, 1>^ newArray() {
	
		return gcnew array<T, 1> { };

	}
	template <typename T> array<T, 1>^ newArray(T a) {
	
		return gcnew array<T, 1> { a };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b) {
	
		return gcnew array<T, 1> { a, b };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c) {
	
		return gcnew array<T, 1> { a, b, c };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c, T d) {
	
		return gcnew array<T, 1> { a, b, c, d };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c, T d, T e) {
	
		return gcnew array<T, 1> { a, b, c, d, e };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c, T d, T e, T f) {
	
		return gcnew array<T, 1> { a, b, c, d, e, f };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c, T d, T e, T f, T g) {
	
		return gcnew array<T, 1> { a, b, c, d, e, f, g };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c, T d, T e, T f, T g, T h) {
	
		return gcnew array<T, 1> { a, b, c, d, e, f, g, h };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c, T d, T e, T f, T g, T h, T i) {
	
		return gcnew array<T, 1> { a, b, c, d, e, f, g, h, i };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j) {
	
		return gcnew array<T, 1> { a, b, c, d, e, f, g, h, i, j };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k) {
	
		return gcnew array<T, 1> { a, b, c, d, e, f, g, h, i, j, k };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l) {
	
		return gcnew array<T, 1> { a, b, c, d, e, f, g, h, i, j, k, l };

	}
	template <typename T> array<T, 1>^ newArray(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m) {
	
		return gcnew array<T, 1> { a, b, c, d, e, f, g, h, i, j, k, l, m };

	}
	template <typename T> array<T, 1>^ newArrayRepeat(T value, int nb) {
		
		array<T, 1>^ ans = gcnew array<T, 1>(nb);

		for (int i = 0; i < nb; i++) {
			ans[i] = value;
		}

		return ans;

	}
	template <typename A, typename B> array<B, 1>^ convert        (array<A, 1>^ arr) {
		
		if (arr == nullptr) return nullptr;

		int len = arr->Length;
		array<B, 1>^ ans = gcnew array<B, 1>(len);

		for (int i = 0; i < len; i++) {
			ans[i] = (B)(arr[i]);
		}

		return ans;

	}
	template <typename T>             array<T, 1>^ copy           (array<T, 1>^ arr) {
		
		if (arr == nullptr) return nullptr;

		int len = arr->Length;
		array<T, 1>^ ans = gcnew array<T, 1>(len);

		for (int i = 0; i < len; i++) {
			ans[i] = arr[i];
		}

		return ans;

	}
	template <typename T>             array<T, 2>^ copy           (array<T, 2>^ arr) {
		
		if (arr == nullptr) return nullptr;
		
		int bound1 = arr->GetUpperBound(0);
		int bound2 = arr->GetUpperBound(1);
		array<T, 2>^ ans = gcnew array<T, 2>(bound1 + 1, bound2 + 1);

		for (int i = 0; i <= bound1; i++) {
			for (int j = 0; j <= bound2; j++) {
				ans[i, j] = arr[i, j];
			}
		}

		return ans;

	}
	template <typename T>             List<T>^     copy           (List<T>^ list) {
		
		if (list == nullptr) return nullptr;

		List<T>^ ans = gcnew List<T>;

		for each (T element in list) {
			ans->Add(element);
		}

		return ans;

	}
	template <typename T>             array<T, 1>^ fill           (T value, int len) {
	
		array<T, 1>^ ans = gcnew array<T, 1>(len);

		for (int i = 0; i < len; i++) {
			ans[i] = value;
		}

		return ans;

	}
	template <typename T>             array<T, 1>^ increaseSize   (array<T, 1>^ arr, int newSize) {
		
		int oldSize = arr->Length;
		if (newSize < oldSize) throw gcnew Exception("The new size is less than the old size.");
		array<T, 1>^ ans = gcnew array<T, 1>(newSize);

		for (int i = 0; i < oldSize; i++) ans[i] = arr[i];

		return ans;

	}
	template <typename T>             array<T, 1>^ reduceSize     (array<T, 1>^ arr, int newSize) {
		
		int oldSize = arr->Length;
		if (newSize > oldSize) throw gcnew Exception("The new size is more than the old size.");
		array<T, 1>^ ans = gcnew array<T, 1>(newSize);

		for (int i = 0; i < newSize; i++) ans[i] = arr[i];

		return ans;

	}
	template <typename T>             void         scramble       (array<T, 1>^ arr) {
		
		int len = arr->Length;

		// Mélange de Knuth
		for (int i = len - 1; i >= 1; i--) {
			int j = Rand::integer(0, i + 1);
			T exch = arr[i];
			arr[i] = arr[j];
			arr[j] = exch  ;
		}

	}
	template <typename T>             array<T, 1>^ concat         (array<T, 1>^ a, array<T, 1>^ b) {
	
		if (a == nullptr || b == nullptr) throw gcnew ArgumentNullException;

		int len1 = a->Length;
		int len2 = b->Length;
		array<T, 1>^ ans = gcnew array<T, 1>(len1 + len2);

		for (int i = 0; i < len1; i++) {
			ans[i] = a[i];
		}
		for (int i = 0; i < len2; i++) {
			ans[i + len1] = b[i];
		}

		return ans;

	}
	template <typename T>             array<T, 1>^ concat         (array<T, 1>^ a, array<T, 1>^ b, array<T, 1>^ c) {
	
		return concat(concat(a, b), c);

	}
	template <typename T>             array<T, 1>^ concat         (array<T, 1>^ a, array<T, 1>^ b, array<T, 1>^ c, array<T, 1>^ d) {
	
		return concat(concat(a, b), concat(c, d));

	}
	bool                                           contains       (array<int, 1>^ arr, int value) {
		
		for each (int i in arr) if (i == value) return true;

		return false;

	}
	bool                                           contains       (List<int>^ lst, int value) {
		
		for each (int i in lst) if (i == value) return true;

		return false;

	}

	// Réflexivité
	public ref class ReflectionTools abstract {
	
		private:
			static array<Type^, 1>^ allTypes = Reflection::Assembly::GetCallingAssembly()->GetTypes();

		public:
			static array<Type^, 1>^ getTypesByName(String^ exactTypeName) {
			
				List<Type^>^ ans = gcnew List<Type^>;

				for each (Type^ type in allTypes) {
					if (type->Name == exactTypeName) {
						ans->Add(type);
					}
				}

				return ans->ToArray();

			}
			static Type^ getTypeByName_throwIfNotUnique(String^ exactTypeName) {
			
				array<Type^, 1>^ ans = getTypesByName(exactTypeName);
				int nb = ans->Length;

				if      (nb == 0) throw gcnew Exception("No matching type.");
				else if (nb == 1) return ans[0];
				else              throw gcnew Exception("Many matching types.");

			}
			static array<Type^, 1>^ getChildrenTypesByParentTypeName(String^ parentExactTypeName, bool keepAbstractClasses) {
			
				Type^ parentType = getTypeByName_throwIfNotUnique(parentExactTypeName);
			
				List<Type^>^ ans = gcnew List<Type^>;
			
				for each (Type^ type in allTypes) {
					if (type->IsSubclassOf(parentType) && (keepAbstractClasses || !type->IsAbstract)) {
						ans->Add(type);
					}
				}

				return ans->ToArray();
			
			}

	};

	// Paire gauche-droite
	template <typename T> public ref struct LeftRight {

		// Champs
		public:
			T left ;
			T right;

		// Constructeur
		public:
			LeftRight(T left, T right) :
				left (left ),
				right(right)
			{ }

	};

}
}
