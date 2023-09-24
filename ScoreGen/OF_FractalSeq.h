#pragma region head

	#pragma once

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	#include "OptionsForm.h"

#pragma endregion

namespace ScoreGen {
	public ref class OF_FractalSeq : public OptionsForm {

	// Constructeur, destructeur
	public:
		OF_FractalSeq(String^ title, String^ description) :
			OptionsForm(title, description)
		{

			String^ text = Text;
		
			InitializeComponent();

			Text = text;

		}
	protected:
		~OF_FractalSeq() {

			if (components) delete components;

		}

	// Evénements
	private:
		void OF_FractalSeq_Load(Object^, EventArgs^) {
		
			comboBoxSequence    ->SelectedIndex = 2;
			comboBoxScale       ->SelectedIndex = 1;
			comboBoxBaseDuration->SelectedIndex = 2;
			comboBoxDurationMult->SelectedIndex = 0;

		}
		void comboBoxSequence_SelectedIndexChanged(Object^, EventArgs^) {
			
			numericUpDownCutRatio->Enabled = comboBoxSequence->SelectedIndex != 0;

			previewSeq();

		}
		void numericUpDownCutRatio_ValueChanged(Object^, EventArgs^) {
		
			previewSeq();

		}
		void comboBoxScale_SelectedIndexChanged(Object^, EventArgs^) {
			
			int selected = comboBoxScale->SelectedIndex;

			switch (selected) {
				case 0 : textBoxCustomScale->Text = "0 0"; break;
				case 1 : textBoxCustomScale->Text = "0 1 2 3 4 5 6 7 8 9 10 11 12"; break;
				case 2 : textBoxCustomScale->Text = "0 2 4 5 7 9 11 12"; break;
				case 3 : textBoxCustomScale->Text = "0 2 3 5 7 8 10 12"; break;
				case 4 : textBoxCustomScale->Text = "0 12"; break;
				case 5 : textBoxCustomScale->Text = "0 1 3 4 6 7 9 10 12"; break;
				case 6 : textBoxCustomScale->Text = "0 12 19 24 28 31 34 36 38 40 42 43 45 46 47 48 49 50 51 52 53 54 55"; break;
				case 7 : textBoxCustomScale->Text = "0 19 28 34 38 42 45 47 49 51 53 55 56 57 58 59 60 61 62 63 64 65 66"; break;
				case 8 : textBoxCustomScale->Text = "0 4 7 10 12"; break;
			}

			textBoxCustomScale->Enabled = selected == comboBoxScale->Items->Count - 1;

		}
		void comboBoxBaseDuration_SelectedIndexChanged(Object^, EventArgs^) {
			
			int selected = comboBoxBaseDuration->SelectedIndex;
		
			switch (selected) {
				case 0 : numericUpDownBaseDuration->Value = 16  ; break;
				case 1 : numericUpDownBaseDuration->Value = 32  ; break;
				case 2 : numericUpDownBaseDuration->Value = 64  ; break;
				case 3 : numericUpDownBaseDuration->Value = 128 ; break;
				case 4 : numericUpDownBaseDuration->Value = 256 ; break;
				case 5 : numericUpDownBaseDuration->Value = 512 ; break;
				case 6 : numericUpDownBaseDuration->Value = 1024; break;
				case 7 : numericUpDownBaseDuration->Value = 2048; break;
				case 8 : numericUpDownBaseDuration->Value = 64  ; break;
			}

			numericUpDownBaseDuration->Enabled = selected == comboBoxBaseDuration->Items->Count - 1;

		}
		void comboBoxDurationMult_SelectedIndexChanged(Object^, EventArgs^) {
			
			int selected = comboBoxDurationMult->SelectedIndex;
		
			switch (selected) {
				case 0 : numericUpDownCustomDurationMult->Value = (Decimal)1      ; break;
				case 1 : numericUpDownCustomDurationMult->Value = (Decimal)0.5    ; break;
				case 2 : numericUpDownCustomDurationMult->Value = (Decimal)0.33333; break;
				case 3 : numericUpDownCustomDurationMult->Value = (Decimal)0.66667; break;
				case 4 : numericUpDownCustomDurationMult->Value = (Decimal)0.75   ; break;
				case 5 : numericUpDownCustomDurationMult->Value = (Decimal)0.83333; break;
				case 6 : numericUpDownCustomDurationMult->Value = (Decimal)2      ; break;
				case 7 : numericUpDownCustomDurationMult->Value = (Decimal)3      ; break;
				case 8 : numericUpDownCustomDurationMult->Value = (Decimal)1.5    ; break;
				case 9 : numericUpDownCustomDurationMult->Value = (Decimal)1.33333; break;
				case 10: numericUpDownCustomDurationMult->Value = (Decimal)1.2    ; break;
				case 11: numericUpDownCustomDurationMult->Value = (Decimal)0.61803; break;
			}

			numericUpDownCustomDurationMult->Enabled = selected == comboBoxDurationMult->Items->Count - 1;

		}

	// Implémentations
	protected:
		virtual array<Object^, 1>^ getOptionsInFormValues() override {
			
			// Calculer la suite fractale demandée
			array<int, 1>^ seq;
			int len;
			{
				len = (int)(numericUpDownNbElements->Value);
				seq = genFractalSequence(
					comboBoxSequence->SelectedIndex,
					(double)(numericUpDownCutRatio->Value),
					len
				);
			}

			// Récupérer l'échelle
			array<int, 1>^ scale;
			int scaleLen;
			int addendum;
			{
				try {
					array<String^, 1>^ values_str = textBoxCustomScale->Text->Split(gcnew array<wchar_t, 1>{L' '}, StringSplitOptions::RemoveEmptyEntries);
					int nb = values_str->Length;
					if (nb <= 1) throw;
					scale = gcnew array<int, 1>(nb - 1);
					scaleLen = nb - 1;
					for (int i = 0; i < scaleLen; i++) {
						scale[i] = Int32::Parse(values_str[i]);
					}
					addendum = Int32::Parse(values_str[nb - 1]);
				} catch (...) {
					MessageBox::Show("Impossible de créer la séquence : le mode personnalisé est incorrect.");
					return gcnew array<Object^, 1>{};
				}
			}

			// Retourner l'échelle si besoin
			if (checkBoxScaleDown->Checked) {
				for (int i = 0; i < scaleLen; i++) {
					scale[i] = -scale[i];
				}
				addendum = -addendum;
			}

			// Calculer les hauteurs MIDI des notes
			array<int, 1>^ pitches; {
				pitches = gcnew array<int, 1>(len);
				int startInScale = (int)(numericUpDownOffsetInScale->Value);
				int MIDIfirstNote = (int)(numericUpDownMIDIfirstNote->Value);
				for (int i = 0; i < len; i++) {
					int seqVal = seq[i];
					int indexInScale = seqVal + startInScale;
					pitches[i] = MIDIfirstNote + scale[indexInScale % scaleLen] + addendum * (indexInScale / scaleLen) - scale[startInScale];
				}
			}

			// Calculer les durées des notes
			array<int, 1>^ durations; {
				// Déterminer la valeur maximale atteinte par la suite
				int maxInSeq = -1;
				for each (int val in seq) if (val > maxInSeq) maxInSeq = val;
				// Créer l'échelle de durées
				array<double, 1>^ durationsScale_dbl; {
					durationsScale_dbl = gcnew array<double, 1>(maxInSeq + 1);
					// Récupérer les options
					int base = (int)(numericUpDownBaseDuration->Value);
					double mult; {
						switch (comboBoxDurationMult->SelectedIndex) {
							case 0 : mult = 1. / 1.; break;
							case 1 : mult = 1. / 2.; break;
							case 2 : mult = 1. / 3.; break;
							case 3 : mult = 2. / 3.; break;
							case 4 : mult = 3. / 4.; break;
							case 5 : mult = 5. / 6.; break;
							case 6 : mult = 2. / 1.; break;
							case 7 : mult = 3. / 1.; break;
							case 8 : mult = 3. / 2.; break;
							case 9 : mult = 4. / 3.; break;
							case 10: mult = 6. / 5.; break;
							case 11: mult = (double)(numericUpDownCustomDurationMult->Value); break;
						}
					}
					// Créer
					durationsScale_dbl[0] = base;
					for (int i = 1; i <= maxInSeq; i++) {
						durationsScale_dbl[i] = mult * durationsScale_dbl[i - 1];
					}
				}
				// Arrondir à 4 près et éviter les 0
				array<int, 1>^ durationsScale; {
					durationsScale = gcnew array<int, 1>(maxInSeq + 1);
					for (int i = 0; i <= maxInSeq; i++) {
						durationsScale[i] = Math::Max(4, 4 * (int)Math::Round(0.25 * durationsScale_dbl[i], 0));
					}
				}
				// Créer les durées de chaque note de la suite
				{
					durations = gcnew array<int, 1>(len);
					for (int i = 0; i < len; i++) {
						durations[i] = durationsScale[seq[i]];
					}
				}
			}

			// Retourner
			return gcnew array<Object^, 1>{
				pitches,
				durations
			};

		}

	// Fonctions
	private:
		void previewSeq() {
			
			// Calculer les premiers éléments de la suite fractale demandée
			int nb = 50;
			array<int, 1>^ seq; {
				seq = genFractalSequence(
					comboBoxSequence->SelectedIndex,
					(double)(numericUpDownCutRatio->Value),
					nb
				);
			}

			// Convertir en chaîne affichable
			System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder;
			for (int i = 0; i < nb; i++) {
				sb->Append((i == 0 ? "" : " ") + (seq[i] + ""));
			}
			sb->Append(" ...");

			// Afficher
			textBoxSeqPreview->Text = sb->ToString();

		}
		static array<int, 1>^ genFractalSequence(int seqDefinition, double cutRatioPercent, int nbElements) {
			
			double cutRatio = 0.01 * cutRatioPercent;

			array<int, 1>^ ans = gcnew array<int, 1>(nbElements);
			ans[0] = 0;

			int maxVal = 0;
			int nextPos = 1;

			while (true) {
				// Inscrire un nombre encore jamais atteint
				if (nextPos == nbElements) break;
				ans[nextPos++] = ++maxVal;
				// Déterminer combien d'éléments recopier
				int nbCopy = (seqDefinition == 0) ? nextPos - 1 : (int)Math::Ceiling(cutRatio * (nextPos - 1));
				if (nbCopy < 1) nbCopy = 1;
				// Déterminer les indices des éléments à recopier
				int beg, end;
				beg = (seqDefinition < 2) ? 0 : nextPos - nbCopy - 1;
				end = beg + nbCopy - 1;
				// Recopier
				for (int i = beg; i <= end; i++) {
					if (nextPos == nbElements) break;
					ans[nextPos++] = ans[i];
				}
			};

			return ans;

		}

	// Contrôles
	#pragma region controls
		private: System::Windows::Forms::GroupBox^  groupBoxSequence;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownNbElements;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::ComboBox^  comboBoxSequence;
private: System::Windows::Forms::NumericUpDown^  numericUpDownCutRatio;


		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::Label^  label3;
		private: System::Windows::Forms::GroupBox^  groupBox1;
		private: System::Windows::Forms::ComboBox^  comboBoxScale;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownOffsetInScale;
		private: System::Windows::Forms::Label^  label4;
		private: System::Windows::Forms::CheckBox^  checkBoxScaleDown;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownMIDIfirstNote;
		private: System::Windows::Forms::Label^  label5;
		private: System::Windows::Forms::GroupBox^  groupBoxDurations;
		private: System::Windows::Forms::ComboBox^  comboBoxBaseDuration;
		private: System::Windows::Forms::TextBox^  textBoxCustomScale;
		private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::NumericUpDown^  numericUpDownBaseDuration;

		private: System::Windows::Forms::Label^  label7;
		private: System::Windows::Forms::ComboBox^  comboBoxDurationMult;
		private: System::Windows::Forms::Label^  label8;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownCustomDurationMult;
		private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::TextBox^  textBoxSeqPreview;

		private:
					System::ComponentModel::Container ^components;
	#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->groupBoxSequence = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxSeqPreview = (gcnew System::Windows::Forms::TextBox());
			this->numericUpDownCutRatio = (gcnew System::Windows::Forms::NumericUpDown());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownNbElements = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->comboBoxSequence = (gcnew System::Windows::Forms::ComboBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxCustomScale = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownMIDIfirstNote = (gcnew System::Windows::Forms::NumericUpDown());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownOffsetInScale = (gcnew System::Windows::Forms::NumericUpDown());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->checkBoxScaleDown = (gcnew System::Windows::Forms::CheckBox());
			this->comboBoxScale = (gcnew System::Windows::Forms::ComboBox());
			this->groupBoxDurations = (gcnew System::Windows::Forms::GroupBox());
			this->numericUpDownCustomDurationMult = (gcnew System::Windows::Forms::NumericUpDown());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->comboBoxDurationMult = (gcnew System::Windows::Forms::ComboBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownBaseDuration = (gcnew System::Windows::Forms::NumericUpDown());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->comboBoxBaseDuration = (gcnew System::Windows::Forms::ComboBox());
			this->groupBoxSequence->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownCutRatio))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbElements))->BeginInit();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMIDIfirstNote))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownOffsetInScale))->BeginInit();
			this->groupBoxDurations->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownCustomDurationMult))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownBaseDuration))->BeginInit();
			this->SuspendLayout();
			// 
			// groupBoxSequence
			// 
			this->groupBoxSequence->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->groupBoxSequence->Controls->Add(this->textBoxSeqPreview);
			this->groupBoxSequence->Controls->Add(this->numericUpDownCutRatio);
			this->groupBoxSequence->Controls->Add(this->label2);
			this->groupBoxSequence->Controls->Add(this->numericUpDownNbElements);
			this->groupBoxSequence->Controls->Add(this->label1);
			this->groupBoxSequence->Controls->Add(this->comboBoxSequence);
			this->groupBoxSequence->Controls->Add(this->label3);
			this->groupBoxSequence->Location = System::Drawing::Point(12, 84);
			this->groupBoxSequence->Name = L"groupBoxSequence";
			this->groupBoxSequence->Size = System::Drawing::Size(199, 213);
			this->groupBoxSequence->TabIndex = 0;
			this->groupBoxSequence->TabStop = false;
			this->groupBoxSequence->Text = L"Suite";
			// 
			// textBoxSeqPreview
			// 
			this->textBoxSeqPreview->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBoxSeqPreview->Location = System::Drawing::Point(6, 96);
			this->textBoxSeqPreview->Multiline = true;
			this->textBoxSeqPreview->Name = L"textBoxSeqPreview";
			this->textBoxSeqPreview->ReadOnly = true;
			this->textBoxSeqPreview->Size = System::Drawing::Size(187, 66);
			this->textBoxSeqPreview->TabIndex = 6;
			this->textBoxSeqPreview->Text = L"(preview)";
			// 
			// numericUpDownCutRatio
			// 
			this->numericUpDownCutRatio->DecimalPlaces = 1;
			this->numericUpDownCutRatio->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
			this->numericUpDownCutRatio->Location = System::Drawing::Point(6, 68);
			this->numericUpDownCutRatio->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
			this->numericUpDownCutRatio->Name = L"numericUpDownCutRatio";
			this->numericUpDownCutRatio->Size = System::Drawing::Size(73, 22);
			this->numericUpDownCutRatio->TabIndex = 4;
			this->numericUpDownCutRatio->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
			this->numericUpDownCutRatio->ValueChanged += gcnew System::EventHandler(this, &OF_FractalSeq::numericUpDownCutRatio_ValueChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(3, 48);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(144, 17);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Ratio de découpage :";
			// 
			// numericUpDownNbElements
			// 
			this->numericUpDownNbElements->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->numericUpDownNbElements->Location = System::Drawing::Point(6, 185);
			this->numericUpDownNbElements->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->numericUpDownNbElements->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->numericUpDownNbElements->Name = L"numericUpDownNbElements";
			this->numericUpDownNbElements->Size = System::Drawing::Size(73, 22);
			this->numericUpDownNbElements->TabIndex = 2;
			this->numericUpDownNbElements->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 165);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(138, 17);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Nombre d\'éléments :";
			// 
			// comboBoxSequence
			// 
			this->comboBoxSequence->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBoxSequence->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBoxSequence->FormattingEnabled = true;
			this->comboBoxSequence->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Répéter tout", L"Répéter 1ère partie", 
				L"Répéter 2nde partie"});
			this->comboBoxSequence->Location = System::Drawing::Point(6, 21);
			this->comboBoxSequence->Name = L"comboBoxSequence";
			this->comboBoxSequence->Size = System::Drawing::Size(187, 24);
			this->comboBoxSequence->TabIndex = 0;
			this->comboBoxSequence->SelectedIndexChanged += gcnew System::EventHandler(this, &OF_FractalSeq::comboBoxSequence_SelectedIndexChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(82, 70);
			this->label3->Margin = System::Windows::Forms::Padding(0, 0, 3, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(20, 17);
			this->label3->TabIndex = 5;
			this->label3->Text = L"%";
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->groupBox1->Controls->Add(this->textBoxCustomScale);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->numericUpDownMIDIfirstNote);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->numericUpDownOffsetInScale);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->checkBoxScaleDown);
			this->groupBox1->Controls->Add(this->comboBoxScale);
			this->groupBox1->Location = System::Drawing::Point(217, 84);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(199, 213);
			this->groupBox1->TabIndex = 3;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Hauteurs de notes";
			// 
			// textBoxCustomScale
			// 
			this->textBoxCustomScale->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBoxCustomScale->Enabled = false;
			this->textBoxCustomScale->Location = System::Drawing::Point(6, 68);
			this->textBoxCustomScale->Name = L"textBoxCustomScale";
			this->textBoxCustomScale->Size = System::Drawing::Size(187, 22);
			this->textBoxCustomScale->TabIndex = 8;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(3, 48);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(136, 17);
			this->label6->TabIndex = 7;
			this->label6->Text = L"Mode personnalisé :";
			// 
			// numericUpDownMIDIfirstNote
			// 
			this->numericUpDownMIDIfirstNote->Location = System::Drawing::Point(6, 158);
			this->numericUpDownMIDIfirstNote->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {127, 0, 0, 0});
			this->numericUpDownMIDIfirstNote->Name = L"numericUpDownMIDIfirstNote";
			this->numericUpDownMIDIfirstNote->Size = System::Drawing::Size(73, 22);
			this->numericUpDownMIDIfirstNote->TabIndex = 6;
			this->numericUpDownMIDIfirstNote->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {60, 0, 0, 0});
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(3, 138);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(184, 17);
			this->label5->TabIndex = 5;
			this->label5->Text = L"Valeur de la première note :";
			// 
			// numericUpDownOffsetInScale
			// 
			this->numericUpDownOffsetInScale->Location = System::Drawing::Point(6, 113);
			this->numericUpDownOffsetInScale->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {48, 0, 0, 0});
			this->numericUpDownOffsetInScale->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {48, 0, 0, System::Int32::MinValue});
			this->numericUpDownOffsetInScale->Name = L"numericUpDownOffsetInScale";
			this->numericUpDownOffsetInScale->Size = System::Drawing::Size(73, 22);
			this->numericUpDownOffsetInScale->TabIndex = 4;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(3, 93);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(159, 17);
			this->label4->TabIndex = 3;
			this->label4->Text = L"Départ dans la gamme :";
			// 
			// checkBoxScaleDown
			// 
			this->checkBoxScaleDown->AutoSize = true;
			this->checkBoxScaleDown->Location = System::Drawing::Point(6, 186);
			this->checkBoxScaleDown->Name = L"checkBoxScaleDown";
			this->checkBoxScaleDown->Size = System::Drawing::Size(152, 21);
			this->checkBoxScaleDown->TabIndex = 2;
			this->checkBoxScaleDown->Text = L"Gamme vers le bas";
			this->checkBoxScaleDown->UseVisualStyleBackColor = true;
			// 
			// comboBoxScale
			// 
			this->comboBoxScale->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBoxScale->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBoxScale->FormattingEnabled = true;
			this->comboBoxScale->Items->AddRange(gcnew cli::array< System::Object^  >(9) {L"Note unique", L"Chromatique", L"Mode majeur", 
				L"Mode de la", L"Octaves", L"Mode II", L"Harmoniques", L"Harmoniques impaires", L"Personnalisé..."});
			this->comboBoxScale->Location = System::Drawing::Point(6, 21);
			this->comboBoxScale->Name = L"comboBoxScale";
			this->comboBoxScale->Size = System::Drawing::Size(187, 24);
			this->comboBoxScale->TabIndex = 1;
			this->comboBoxScale->SelectedIndexChanged += gcnew System::EventHandler(this, &OF_FractalSeq::comboBoxScale_SelectedIndexChanged);
			// 
			// groupBoxDurations
			// 
			this->groupBoxDurations->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->groupBoxDurations->Controls->Add(this->numericUpDownCustomDurationMult);
			this->groupBoxDurations->Controls->Add(this->label9);
			this->groupBoxDurations->Controls->Add(this->comboBoxDurationMult);
			this->groupBoxDurations->Controls->Add(this->label8);
			this->groupBoxDurations->Controls->Add(this->numericUpDownBaseDuration);
			this->groupBoxDurations->Controls->Add(this->label7);
			this->groupBoxDurations->Controls->Add(this->comboBoxBaseDuration);
			this->groupBoxDurations->Location = System::Drawing::Point(422, 84);
			this->groupBoxDurations->Name = L"groupBoxDurations";
			this->groupBoxDurations->Size = System::Drawing::Size(198, 213);
			this->groupBoxDurations->TabIndex = 4;
			this->groupBoxDurations->TabStop = false;
			this->groupBoxDurations->Text = L"Durée des notes";
			// 
			// numericUpDownCustomDurationMult
			// 
			this->numericUpDownCustomDurationMult->DecimalPlaces = 5;
			this->numericUpDownCustomDurationMult->Enabled = false;
			this->numericUpDownCustomDurationMult->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 131072});
			this->numericUpDownCustomDurationMult->Location = System::Drawing::Point(6, 160);
			this->numericUpDownCustomDurationMult->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
			this->numericUpDownCustomDurationMult->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 65536});
			this->numericUpDownCustomDurationMult->Name = L"numericUpDownCustomDurationMult";
			this->numericUpDownCustomDurationMult->Size = System::Drawing::Size(73, 22);
			this->numericUpDownCustomDurationMult->TabIndex = 10;
			this->numericUpDownCustomDurationMult->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(3, 140);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(184, 17);
			this->label9->TabIndex = 9;
			this->label9->Text = L"Multiplicateur personnalisé :";
			// 
			// comboBoxDurationMult
			// 
			this->comboBoxDurationMult->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBoxDurationMult->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBoxDurationMult->FormattingEnabled = true;
			this->comboBoxDurationMult->Items->AddRange(gcnew cli::array< System::Object^  >(12) {L"Constant", L"1:2", L"1:3", L"2:3", 
				L"3:4", L"5:6", L"2:1", L"3:1", L"3:2", L"4:3", L"6:5", L"Personnalisé..."});
			this->comboBoxDurationMult->Location = System::Drawing::Point(6, 113);
			this->comboBoxDurationMult->Name = L"comboBoxDurationMult";
			this->comboBoxDurationMult->Size = System::Drawing::Size(186, 24);
			this->comboBoxDurationMult->TabIndex = 8;
			this->comboBoxDurationMult->SelectedIndexChanged += gcnew System::EventHandler(this, &OF_FractalSeq::comboBoxDurationMult_SelectedIndexChanged);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(3, 93);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(140, 17);
			this->label8->TabIndex = 7;
			this->label8->Text = L"Multiplicateur durée :";
			// 
			// numericUpDownBaseDuration
			// 
			this->numericUpDownBaseDuration->Enabled = false;
			this->numericUpDownBaseDuration->Location = System::Drawing::Point(6, 68);
			this->numericUpDownBaseDuration->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65536, 0, 0, 0});
			this->numericUpDownBaseDuration->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
			this->numericUpDownBaseDuration->Name = L"numericUpDownBaseDuration";
			this->numericUpDownBaseDuration->Size = System::Drawing::Size(73, 22);
			this->numericUpDownBaseDuration->TabIndex = 6;
			this->numericUpDownBaseDuration->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {64, 0, 0, 0});
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(3, 48);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(158, 17);
			this->label7->TabIndex = 5;
			this->label7->Text = L"En 256èmes de noires :";
			// 
			// comboBoxBaseDuration
			// 
			this->comboBoxBaseDuration->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBoxBaseDuration->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBoxBaseDuration->FormattingEnabled = true;
			this->comboBoxBaseDuration->Items->AddRange(gcnew cli::array< System::Object^  >(9) {L"Quadruple croche", L"Triple croche", 
				L"Double croche", L"Croche", L"Noire", L"Blanche", L"Ronde", L"Note carrée", L"Personnalisé..."});
			this->comboBoxBaseDuration->Location = System::Drawing::Point(6, 21);
			this->comboBoxBaseDuration->Name = L"comboBoxBaseDuration";
			this->comboBoxBaseDuration->Size = System::Drawing::Size(186, 24);
			this->comboBoxBaseDuration->TabIndex = 2;
			this->comboBoxBaseDuration->SelectedIndexChanged += gcnew System::EventHandler(this, &OF_FractalSeq::comboBoxBaseDuration_SelectedIndexChanged);
			// 
			// OF_FractalSeq
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(632, 347);
			this->Controls->Add(this->groupBoxDurations);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->groupBoxSequence);
			this->Name = L"OF_FractalSeq";
			this->Text = L"OF_FractalSeq";
			this->Load += gcnew System::EventHandler(this, &OF_FractalSeq::OF_FractalSeq_Load);
			this->Controls->SetChildIndex(this->groupBoxSequence, 0);
			this->Controls->SetChildIndex(this->groupBox1, 0);
			this->Controls->SetChildIndex(this->groupBoxDurations, 0);
			this->groupBoxSequence->ResumeLayout(false);
			this->groupBoxSequence->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownCutRatio))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbElements))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMIDIfirstNote))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownOffsetInScale))->EndInit();
			this->groupBoxDurations->ResumeLayout(false);
			this->groupBoxDurations->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownCustomDurationMult))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownBaseDuration))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	};
}
