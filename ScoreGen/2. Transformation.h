#pragma region head

	#pragma once

	using namespace System;

#pragma endregion

namespace ScoreGenLib {
namespace Music {
namespace Transformations {
	
	// Transformation musicale (classe de base)
	public ref class Transformation abstract {
		
		// A implémenter
		public:
			virtual bool isIdentity      () = 0;
			virtual bool isTransposition () = 0;
			virtual bool isInversion     () = 0;
			virtual bool isRetrograde    () = 0;
			virtual bool isAugmentation  () = 0;
			virtual bool isDiminution    () = 0;
			virtual bool isMultiplication() = 0;
			virtual bool isDivision      () = 0;
			virtual bool isCompound      () = 0;
			virtual Transformation^ inverse() = 0; // utilisée par 'operator-'

		// Opérateurs : inversion et composition de transformations
		public:
			inline Transformation^ operator+() { return this; }
			inline Transformation^ operator-() { return this->inverse(); }
			static Transformation^ operator+(Transformation^ a, Transformation^ b);
			static inline Transformation^ operator-(Transformation^ a, Transformation^ b) {
			
				return a + (-b);
			
			}

	};

	// Transformations musicales individuelles...
	public ref class Identity : public Transformation {
		
		// Implémentations
		public:
			virtual bool isIdentity      () override { return true ; }
			virtual bool isTransposition () override { return false; }
			virtual bool isInversion     () override { return false; }
			virtual bool isRetrograde    () override { return false; }
			virtual bool isAugmentation  () override { return false; }
			virtual bool isDiminution    () override { return false; }
			virtual bool isMultiplication() override { return false; }
			virtual bool isDivision      () override { return false; }
			virtual bool isCompound      () override { return false; }
			virtual Transformation^ inverse() override {
				
				return this;

			}

	};
	public ref class Transposition : public Transformation {
		
		// Champs
		private:
			double _offset;

		// Constructeurs
		public:
			Transposition(double offset) :
				_offset(offset)
			{ }

		// Accesseurs
		public:
			property double offset {
				inline double get() {
					return _offset;
				}
			}

		// Implémentations
		public:
			virtual bool isIdentity      () override { return false; }
			virtual bool isTransposition () override { return true ; }
			virtual bool isInversion     () override { return false; }
			virtual bool isRetrograde    () override { return false; }
			virtual bool isAugmentation  () override { return false; }
			virtual bool isDiminution    () override { return false; }
			virtual bool isMultiplication() override { return false; }
			virtual bool isDivision      () override { return false; }
			virtual bool isCompound      () override { return false; }
			virtual Transformation^ inverse() override {
				
				return gcnew Transposition(-_offset);

			}

	};
	public ref class Inversion : public Transformation {
		
		// Champs
		private:
			double _center;

		// Constructeurs
		public:
			Inversion(double center) :
				_center(center)
			{ }
			Inversion(double midiSource, double midiTarget) :
				_center(0.5 * (midiSource + midiTarget))
			{ }
			Inversion() :
				_center(60)
			{ }

		// Accesseurs
		public:
			property double center {
				inline double get() {
					return _center;
				}
			}

		// Implémentations
		public:
			virtual bool isIdentity      () override { return false; }
			virtual bool isTransposition () override { return false; }
			virtual bool isInversion     () override { return true ; }
			virtual bool isRetrograde    () override { return false; }
			virtual bool isAugmentation  () override { return false; }
			virtual bool isDiminution    () override { return false; }
			virtual bool isMultiplication() override { return false; }
			virtual bool isDivision      () override { return false; }
			virtual bool isCompound      () override { return false; }
			virtual Transformation^ inverse() override {
				
				return gcnew Identity;

			}

	};
	public ref class Retrograde : public Transformation {
		
		// Implémentations
		public:
			virtual bool isIdentity      () override { return false; }
			virtual bool isTransposition () override { return false; }
			virtual bool isInversion     () override { return false; }
			virtual bool isRetrograde    () override { return true ; }
			virtual bool isAugmentation  () override { return false; }
			virtual bool isDiminution    () override { return false; }
			virtual bool isMultiplication() override { return false; }
			virtual bool isDivision      () override { return false; }
			virtual bool isCompound      () override { return false; }
			virtual Transformation^ inverse() override {
				
				return gcnew Identity;

			}

	};
	public ref class Augmentation : public Transformation {
		
		// Champs
		private:
			Fraction^ _ratio;

		// Constructeurs
		public:
			Augmentation(Fraction^ ratio) :
				_ratio(ratio)
			{
			
				if (_ratio <= 0) throw gcnew Exception("The ratio should be positive.");
			
			}
			Augmentation(int ratioNumerator, int ratioDenominator) :
				_ratio(gcnew Fraction(ratioNumerator, ratioDenominator))
			{
			
				if (_ratio <= 0) throw gcnew Exception("The ratio should be positive.");

			}

		// Accesseurs
		public:
			property Fraction^ ratio {
				inline Fraction^ get() {
					return _ratio;
				}
			}

		// Implémentations
		public:
			virtual bool isIdentity      () override { return false; }
			virtual bool isTransposition () override { return false; }
			virtual bool isInversion     () override { return false; }
			virtual bool isRetrograde    () override { return false; }
			virtual bool isAugmentation  () override { return true ; }
			virtual bool isDiminution    () override { return false; }
			virtual bool isMultiplication() override { return false; }
			virtual bool isDivision      () override { return false; }
			virtual bool isCompound      () override { return false; }
			virtual Transformation^ inverse() override;

	};
	public ref class Diminution : public Transformation {
		
		// Champs
		private:
			Fraction^ _ratio;

		// Constructeurs
		public:
			Diminution(Fraction^ ratio) :
				_ratio(ratio)
			{
			
				if (_ratio <= 0) throw gcnew Exception("The ratio should be positive.");
			
			}
			Diminution(int ratioNumerator, int ratioDenominator) :
				_ratio(gcnew Fraction(ratioNumerator, ratioDenominator))
			{
			
				if (_ratio <= 0) throw gcnew Exception("The ratio should be positive.");

			}

		// Accesseurs
		public:
			property Fraction^ ratio {
				inline Fraction^ get() {
					return _ratio;
				}
			}

		// Implémentations
		public:
			virtual bool isIdentity      () override { return false; }
			virtual bool isTransposition () override { return false; }
			virtual bool isInversion     () override { return false; }
			virtual bool isRetrograde    () override { return false; }
			virtual bool isAugmentation  () override { return false; }
			virtual bool isDiminution    () override { return true ; }
			virtual bool isMultiplication() override { return false; }
			virtual bool isDivision      () override { return false; }
			virtual bool isCompound      () override { return false; }
			virtual Transformation^ inverse() override {
				
				return gcnew Augmentation(_ratio);

			}

	};
	public ref class Multiplication : public Transformation {
		
		// Champs
		private:
			double _center;
			double _ratio;

		// Constructeurs
		public:
			Multiplication(double center, double ratio) :
				_center(center),
				_ratio(ratio)
			{ }

		// Accesseurs
		public:
			property double center {
				inline double get() {
					return _center;
				}
			}
			property double ratio {
				inline double get() {
					return _ratio;
				}
			}

		// Implémentations
		public:
			virtual bool isIdentity      () override { return false; }
			virtual bool isTransposition () override { return false; }
			virtual bool isInversion     () override { return false; }
			virtual bool isRetrograde    () override { return false; }
			virtual bool isAugmentation  () override { return false; }
			virtual bool isDiminution    () override { return false; }
			virtual bool isMultiplication() override { return true ; }
			virtual bool isDivision      () override { return false; }
			virtual bool isCompound      () override { return false; }
			virtual Transformation^ inverse() override;

	};
	public ref class Division : public Transformation {
		
		// Champs
		private:
			double _center;
			double _ratio;

		// Constructeurs
		public:
			Division(double center, double ratio) :
				_center(center),
				_ratio(ratio)
			{
			
				if (_ratio == 0) throw gcnew Exception("The ratio should not be zero.");

			}

		// Accesseurs
		public:
			property double center {
				inline double get() {
					return _center;
				}
			}
			property double ratio {
				inline double get() {
					return _ratio;
				}
			}

		// Implémentations
		public:
			virtual bool isIdentity      () override { return false; }
			virtual bool isTransposition () override { return false; }
			virtual bool isInversion     () override { return false; }
			virtual bool isRetrograde    () override { return false; }
			virtual bool isAugmentation  () override { return false; }
			virtual bool isDiminution    () override { return false; }
			virtual bool isMultiplication() override { return false; }
			virtual bool isDivision      () override { return true ; }
			virtual bool isCompound      () override { return false; }
			virtual Transformation^ inverse() override {
				
				return gcnew Multiplication(_center, _ratio);

			}

	};
	
	// ...et transformation permettant de composer récursivement plusieurs transformations (aussi accessible par 'operator+')
	public ref class Compound : public Transformation {
		
		// Champs
		private:
			Transformation^ _a;
			Transformation^ _b;

		// Constructeurs
		public:
			Compound(Transformation^ a, Transformation^ b) :
				_a(a),
				_b(b)
			{ }

		// Accesseurs
		public:
			property Transformation^ a {
				inline Transformation^ get() {
					return _a;
				}
			}
			property Transformation^ b {
				inline Transformation^ get() {
					return _b;
				}
			}

		// Implémentations
		public:
			virtual bool isIdentity      () override { return false; }
			virtual bool isTransposition () override { return false; }
			virtual bool isInversion     () override { return false; }
			virtual bool isRetrograde    () override { return false; }
			virtual bool isAugmentation  () override { return false; }
			virtual bool isDiminution    () override { return false; }
			virtual bool isMultiplication() override { return false; }
			virtual bool isDivision      () override { return false; }
			virtual bool isCompound      () override { return true ; }
			virtual Transformation^ inverse() override {
				
				// L'inverse d'une composée est la composée des inverses... dans l'ordre inverse
				return gcnew Compound(_b->inverse(), _a->inverse());

			}

	};

	// Classe abstraite décrivant un objet pouvant être transformé (par exemple, une note ou un tuplet)
	template<typename T> public ref class Transformable abstract {
		
		// Fonctions publiques
		public:
			T transform(Transformation^ transformation) {
				
				if (transformation->isIdentity()) {
					// Identité : retourner l'objet actuel
					return (T)this;
				} else if (transformation->isTransposition() || transformation->isInversion() || transformation->isRetrograde() || transformation->isAugmentation() || transformation->isDiminution() || transformation->isMultiplication() || transformation->isDivision()) { // division
					// Transformation dépendant de l'objet considéré : déléguer le calcul à la classe fille
					return this->transform_protected(transformation);
				} else if (transformation->isCompound()) {
					// Transformation composée : appliquer toutes les transformations successivement
					T ans = (T)this;
					Compound^ composed = (Compound^)transformation;
					ans = ans->transform(composed->a);
					ans = ans->transform(composed->b);
						int PROV = 0; // ordre : a>>>b ? b>>>a ?
					return ans;
				} else {
					// Autre transformation : erreur due à un utilisateur essayant d'utiliser sa propre classe Transformation...
					throw gcnew Exception;
				}

			}

		// A implémenter
		protected:
			virtual T transform_protected(Transformation^ transformation) = 0;

	};

	// Fonctions devant être définies séparément
	Transformation^ Transformation::operator+(Transformation^ a, Transformation^ b) {
		
		return gcnew Compound(a, b);
	
	}
	Transformation^ Augmentation::inverse() {
				
		return gcnew Diminution(_ratio);

	}
	Transformation^ Multiplication::inverse() {
				
		return gcnew Division(_center, _ratio);

	}

}
}
}
