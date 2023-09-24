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
	public ref class OF_SquareFreeSequenceInRhythm : public OptionsForm {

	// Constructeur, destructeur
	public:
		OF_SquareFreeSequenceInRhythm(String^ title, String^ description) :
			OptionsForm(title, description)
		{
			
			String^ text = Text;

			InitializeComponent();

			Text = text;

		}
	protected:
		~OF_SquareFreeSequenceInRhythm() {

			if (components) delete components;

		}

	// Implémentations
	protected:
		virtual array<Object^, 1>^ getOptionsInFormValues() override {

			return gcnew array<Object^, 1> {
				(int)(numericUpDownNbNotes->Value),
				getDurations(textBoxDurations->Text)
			};

		}

	// Evénements
	private:
		void textBoxDurations_TextChanged(Object^, EventArgs^) {
		
			bool ok = getDurations(textBoxDurations->Text) != nullptr;

			setOkButtonEnabled(ok);
			textBoxDurations->BackColor = ok ? Color::White : Color::FromArgb(255, 220, 220);

		}

	// Fonctions
	private:
		static array<int, 1>^ getDurations(String^ str) {
			
			array<String^, 1>^ durations_str = str->Split(gcnew array<wchar_t, 1>{ L' ' }, StringSplitOptions::RemoveEmptyEntries);
			int nb = durations_str->Length;
			
			if (nb < 2 || nb > 4) return nullptr;

			array<int, 1>^ ans = gcnew array<int, 1>(nb);

			for (int i = 0; i < nb; i++) {
				int dur;
				if (!Int32::TryParse(durations_str[i], dur)) return nullptr;
				if (dur < 1 || dur > 1000) return nullptr;
				ans[i] = dur;
			}

			return ans;

		}

	// Contrôles
	#pragma region controls
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownNbNotes;
		private: System::Windows::Forms::GroupBox^  groupBoxOptions;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::TextBox^  textBoxDurations;
		private:
				System::ComponentModel::Container ^components;
	#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownNbNotes = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBoxOptions = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxDurations = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbNotes))->BeginInit();
			this->groupBoxOptions->SuspendLayout();
			this->SuspendLayout();
			// 
			// label2
			// 
			this->label2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(47, 53);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(125, 17);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Nombre de notes :";
			// 
			// numericUpDownNbNotes
			// 
			this->numericUpDownNbNotes->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownNbNotes->Location = System::Drawing::Point(189, 51);
			this->numericUpDownNbNotes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {20000, 0, 0, 0});
			this->numericUpDownNbNotes->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownNbNotes->Name = L"numericUpDownNbNotes";
			this->numericUpDownNbNotes->Size = System::Drawing::Size(103, 22);
			this->numericUpDownNbNotes->TabIndex = 5;
			this->numericUpDownNbNotes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
			// 
			// groupBoxOptions
			// 
			this->groupBoxOptions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxOptions->Controls->Add(this->textBoxDurations);
			this->groupBoxOptions->Controls->Add(this->label1);
			this->groupBoxOptions->Controls->Add(this->numericUpDownNbNotes);
			this->groupBoxOptions->Controls->Add(this->label2);
			this->groupBoxOptions->Location = System::Drawing::Point(12, 84);
			this->groupBoxOptions->Name = L"groupBoxOptions";
			this->groupBoxOptions->Size = System::Drawing::Size(338, 153);
			this->groupBoxOptions->TabIndex = 6;
			this->groupBoxOptions->TabStop = false;
			this->groupBoxOptions->Text = L"Options";
			// 
			// textBoxDurations
			// 
			this->textBoxDurations->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->textBoxDurations->Location = System::Drawing::Point(189, 79);
			this->textBoxDurations->Name = L"textBoxDurations";
			this->textBoxDurations->Size = System::Drawing::Size(103, 22);
			this->textBoxDurations->TabIndex = 8;
			this->textBoxDurations->Text = L"3 1 2";
			this->textBoxDurations->TextChanged += gcnew System::EventHandler(this, &OF_SquareFreeSequenceInRhythm::textBoxDurations_TextChanged);
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(47, 82);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(136, 17);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Durées en doubles :";
			// 
			// OF_SquareFreeSequenceInRythm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(362, 287);
			this->Controls->Add(this->groupBoxOptions);
			this->Name = L"OF_SquareFreeSequenceInRythm";
			this->Text = L"OF_Echange";
			this->Controls->SetChildIndex(this->groupBoxOptions, 0);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbNotes))->EndInit();
			this->groupBoxOptions->ResumeLayout(false);
			this->groupBoxOptions->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	};
}
