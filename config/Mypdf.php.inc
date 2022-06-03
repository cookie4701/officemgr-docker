<?php

namespace App\Libraries;

use \CodeIgniter\I18n\Time;
use TCPDF;

class Mypdf extends TCPDF {
  protected $invoice;
  protected $invoice_items;
  protected $invoice_total_no_vat;
  protected $invoice_total_vat;
  protected $file_name;

  public function Footer() {
    $this->setY(-25);

    $this->Cell(0, 15, 'Name und Unternehmensnummer', 0, 1, 'C', false, '', 0, false, 'C', 'C');
    $this->Cell(0, 10, 'Seite '.$this->getAliasNumPage().' von '.$this->getAliasNbPages(), 0, false, 'C', 0, '', 0, false, 'T', 'M');
  }

  public function setTotal($priceWithoutVat, $priceWithVat)
  {
    $this->invoice_total_no_vat = $priceWithoutVat;
    $this->invoice_total_vat = $priceWithVat;
  }

  public function setFileName($fname) {
    $this->file_name = $fname;
  }

  public function __construct()
  {
    $this->invoice               = null;
    $this->invoice_items         = array();
    $this->invoice_total_no_vat = 0.0;
    $this->invoice_total_vat    = 0.0;
    $this->file_name = 'temp.pdf';

    parent::__construct(PDF_PAGE_ORIENTATION, PDF_UNIT, PDF_PAGE_FORMAT, true, 'UTF-8', false);

    $this->SetCreator(PDF_CREATOR);
    $this->SetAuthor('OFFICEMANAGER');
    $this->SetTitle('Rechnung');
    $this->SetSubject('Rechnung für Dienstleistungen und Kleinmaterialien');
    $this->SetKeywords('Rechnung');

    //Disable header - not needed for this application
    $this->setPrintHeader(false);

    // Footer
    $this->setPrintFooter(true);
    $this->SetFooterMargin(PDF_MARGIN_FOOTER);

    // Font document
    $this->setFont('dejavusans', '', 12, '', true);
    $this->setJPEGQuality(75);

    // set cell padding
    $this->setCellPaddings(1, 1, 1, 1);

    // set cell margins
    $this->setCellMargins(1, 1, 1, 1);
  }

  public function setInvoice($inv)
  {
    //TODO: check if $inv is of type of Invoice (Entity)
    $this->invoice = $inv;
  }

  public function setInvoiceItems($items)
  {
    $this->invoice_items = $items;
  }

  public function toHtml()
  {
    if (! $this->invoice ) return;

    $this->AddPage();

    // Left side, own data

    // Logo
    $img = file_get_contents( FCPATH . 'images/logo.png');

    $this->Image('@' . $img, $x=15.0, $y=15.0, $type="png", $w=40.0, $dpi=75, $resize=false, $hidden=false );





    // Text
    $mdate = Time::parse($this->invoice->invoice_date, 'America/Chicago', 'en_US');

    $this->Cell(0, 10, 'ORT, ' . $mdate->toLocalizedString('dd.MM.YYYY') , 0, 1, 'R', false, '', 0, false, 'C', 'C');
    $this->Cell(0, 100, '', 0, 1, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(0, 10, 'NAME', 0, 2, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(0, 10, 'STRASSEUNDNUMMER', 0, 2, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(0, 10, 'PLZORT', 0, 2, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(0, 10, 'Tel.: TELNUMMER', 0, 2, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(0, 10, 'EMAILADRESSE', 0, 2, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(0, 10, '', 0, 1, 'L', false, '', 0, false, 'C', 'C');

    $this->Cell(100, 10, '', 0, 0, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(0, 10, $this->invoice->rcpt_orga, 0, 2, 'L', false, '', 0, false, 'C', 'C');

    if ($this->invoice->rcpt_contact != '')
      $this->Cell(0, 10, 'c/o ' . $this->invoice->rcpt_contact, 0, 2, 'L', false, '', 0, false, 'C', 'C');

    if ($this->invoice->rcpt_address1 != '')
      $this->Cell(0, 10,$this->invoice->rcpt_address1 , 0, 2, 'L', false, '', 0, false, 'C', 'C');
    if ($this->invoice->rcpt_address2 != '')
      $this->Cell(0, 10, $this->invoice->rcpt_address2, 0, 2, 'L', false, '', 0, false, 'C', 'C');

    if ($this->invoice->rcpt_zip != '' || $this->invoice->rcpt_city != '')
      $this->Cell(0, 10, $this->invoice->rcpt_zip . ' ' . $this->invoice->rcpt_city, 0, 1, 'L', false, '', 0, false, 'C', 'C');

    if ($this->invoice->rcpt_country != '' && $this->invoice->rcpt_country != 'Belgien')
      $this->Cell(0, 10, $this->invoice->rcpt_country, 0, 1, 'L', false, '', 0, false, 'C', 'C');
    else
      $this->Cell(0, 10, '', 0, 1, 'L', false, '', 0, false, 'C', 'C');

    $this->Cell(0, 2, '', 0, 1, 'L', false, '', 0, false, 'C', 'C');

    $this->Cell(0, 10, 'Rechnung', 0, 1, 'C', false, '', 0, false, 'C', 'C');


    $this->Cell(0, 15, $this->invoice->invoice_number, 0, 1, 'C', false, '', 0, false, 'C', 'C');

    // Listing of positions
    $this->Cell(0, 10, 'Leistungen', 0, 1, 'L', false, '', 0, false, 'C', 'C');

    // Kopf
    $this->Cell(20, 10, 'Position', 0, 0, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(90, 10, 'Beschreibung', 0, 0, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(20, 10, 'Anz.', 0, 0, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(30, 10, 'Stückpreis', 0, 0, 'L', false, '', 0, false, 'C', 'C');
    $this->Cell(20, 10, 'Total', 0, 1, 'L', false, '', 0, false, 'C', 'C');

    $total_price = 0.0;
    $item_count = count($this->invoice_items);
    $item_index = 0;
    $firstPage = true;

    foreach ($this->invoice_items as $item)
    {
      $tprice = $item->amount * $item->unit_price;
      $total_price += $tprice;
      $this->Cell(20, 10, $item->position, 0, 0, 'L', false, '', 0, true, 'C', 'C');
      $this->MultiCell(90, 10, $item->label, 0, 'L', false, 0);
      $this->Cell(20, 10, number_format($item->amount, 2, ',' ,'.'), 0, 0, 'L', false, '', 0, true, 'C', 'C');
      $this->Cell(30, 10, number_format($item->unit_price, 2, ',' ,'.'), 0, 0, 'L', false, '', 0, true, 'C', 'C');
      $this->Cell(20, 10, number_format($tprice, 2, ',' ,'.'), 0, 1, 'L', false, '', 0, true, 'C', 'C');
      $this->Ln(10);
      $item_index++;

      if ( (($firstPage && $item_index > 6) || ($item_index - 6) % 12 == 11 ) && $item_index < $item_count) {
        $firstPage = false;
        $this->Cell(0,15,
          "Zwischentotal (Fortsetzung auf der nächsten Seite): EUR " . number_format($total_price, 2, ',' , '.') ,
          0, 1, 'R', false, '', 0, true, 'C', 'C');

        $this->AddPage();

        $this->Cell(0,30, '', 0,1);

        $this->Cell(0,15,
          'Rechnung ' . $this->invoice->invoice_number . ' (Fortsetzung)' ,
          0, 1, 'L', false, '', 0, true, 'C', 'C');

        $this->Cell(0,15,
          "Übertrag von vorheriger Seite: EUR " . number_format($total_price, 2, ',' , '.') ,
          0, 1, 'R', false, '', 0, true, 'C', 'C');

      }
    }

    // End listing of positions

    $this->Cell(0,15,
      "Gesammtwert EUR " . number_format($total_price, 2, ',' , '.') ,
      0, 1, 'R', false, '', 0, true, 'C', 'C');

    if ($total_price > 0) {
      $this->MultiCell(0,15,
      "Bitte überweisen Sie den Betrag, innerhalb von 10 Tagen ab Rechnungsdatum auf das untenstehende Konto!\n" .
      "BANK1 \n" .
      "BANK2 \n" .
      "BANK3",
      0, 'L'
    );
  } else {
    $this->MultiCell(0,10, "Der Betrag wird in den kommenden Tagen auf Ihr Konto überwiesen.",0,'L');
  }

  if ( isset($this->file_name) ) {
    $this->Output($this->file_name, 'D');
  } else {
    $this->Output('noname.pdf', 'D');
  }


    return true;
  }
}

?>
