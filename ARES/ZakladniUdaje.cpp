//
// Created by Petr Flajsingr on 2019-04-26.
//

#include "ZakladniUdaje.h"

ZakladniUdaje::ZakladniUdaje(const pt::ptree &ptree) {
  auto zaklUdaje =
      ptree.get_child("are:Ares_odpovedi").get_child("are:Odpoved").get_child("are:Vypis_VREO").get_child(
          "are:Zakladni_udaje");

  try { ICO = zaklUdaje.get_child("are:ICO").get_value<std::string>(); } catch (...) {}
  try { ObchodniFirma = zaklUdaje.get_child("are:ObchodniFirma").get_value<std::string>(); } catch (...) {}
  try { Kod_PF = zaklUdaje.get_child("are:Kod_PF").get_value<std::string>(); } catch (...) {}
  try { Nazev_PF = zaklUdaje.get_child("are:Nazev_PF").get_value<std::string>(); } catch (...) {}
  try {
    sidlo_ruianKod = zaklUdaje.get_child("are:Sidlo").get_child("are:ruianKod").get_value<std::string>();
  } catch (...) {}
  try {
    sidlo_stat = zaklUdaje.get_child("are:Sidlo").get_child("are:stat").get_value<std::string>();
  } catch (...) {}
  try { sidlo_psc = zaklUdaje.get_child("are:Sidlo").get_child("are:psc").get_value<std::string>(); } catch (
      ...) {}
  try {
    sidlo_okres = zaklUdaje.get_child("are:Sidlo").get_child("are:okres").get_value<std::string>();
  } catch (...) {}
  try {
    sidlo_obec = zaklUdaje.get_child("are:Sidlo").get_child("are:obec").get_value<std::string>();
  } catch (...) {}
  try {
    sidlo_castObce = zaklUdaje.get_child("are:Sidlo").get_child("are:castObce").get_value<std::string>();
  } catch (...) {}
  try { sidlo_mop = zaklUdaje.get_child("are:Sidlo").get_child("are:mop").get_value<std::string>(); } catch (
      ...) {}
  try {
    sidlo_ulice = zaklUdaje.get_child("are:Sidlo").get_child("are:ulice").get_value<std::string>();
  } catch (...) {}
  try {
    sidlo_cisloPop = zaklUdaje.get_child("are:Sidlo").get_child("are:cisloPop").get_value<std::string>();
  } catch (...) {}
  try {
    sidlo_cisloOr = zaklUdaje.get_child("are:Sidlo").get_child("are:cisloOr").get_value<std::string>();
  } catch (...) {}
  try {
    sidlo_cisloEv = zaklUdaje.get_child("are:Sidlo").get_child("are:cisloEv").get_value<std::string>();
  } catch (...) {}
  try {
    sidlo_cisloTxt = zaklUdaje.get_child("are:Sidlo").get_child("are:cisloTxt").get_value<std::string>();
  } catch (...) {}
  try {
    sidlo_doplnujiciText =
        zaklUdaje.get_child("are:Sidlo").get_child("are:doplnujiciText").get_value<std::string>();
  } catch (...) {}
  try {
    sidlo_dorucovaciCislo =
        zaklUdaje.get_child("are:Sidlo").get_child("are:dorucovaciCislo").get_value<std::string>();
  } catch (...) {}
  try {
    sidlo_skrytyUdaj =
        zaklUdaje.get_child("are:Sidlo").get_child("are:skrytyUdaj").get_child("are:text").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_ruianKod =
        zaklUdaje.get_child("are:Bydliste").get_child("are:ruianKod").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_stat = zaklUdaje.get_child("are:Bydliste").get_child("are:stat").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_psc = zaklUdaje.get_child("are:Bydliste").get_child("are:psc").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_okres = zaklUdaje.get_child("are:Bydliste").get_child("are:okres").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_obec = zaklUdaje.get_child("are:Bydliste").get_child("are:obec").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_castObce =
        zaklUdaje.get_child("are:Bydliste").get_child("are:castObce").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_mop = zaklUdaje.get_child("are:Bydliste").get_child("are:mop").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_ulice = zaklUdaje.get_child("are:Bydliste").get_child("are:ulice").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_cisloPop =
        zaklUdaje.get_child("are:Bydliste").get_child("are:cisloPop").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_cisloOr = zaklUdaje.get_child("are:Bydliste").get_child("are:cisloOr").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_cisloEv = zaklUdaje.get_child("are:Bydliste").get_child("are:cisloEv").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_cisloTxt =
        zaklUdaje.get_child("are:Bydliste").get_child("are:cisloTxt").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_doplnujiciText =
        zaklUdaje.get_child("are:Bydliste").get_child("are:doplnujiciText").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_dorucovaciCislo =
        zaklUdaje.get_child("are:Bydliste").get_child("are:dorucovaciCislo").get_value<std::string>();
  } catch (...) {}
  try {
    bydliste_skrytyUdaj =
        zaklUdaje.get_child("are:Bydliste").get_child("are:skrytyUdaj").get_child("are:text").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_ruianKod = zaklUdaje.get_child("are:Pobyt").get_child("are:ruianKod").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_stat = zaklUdaje.get_child("are:Pobyt").get_child("are:stat").get_value<std::string>();
  } catch (...) {}
  try { pobyt_psc = zaklUdaje.get_child("are:Pobyt").get_child("are:psc").get_value<std::string>(); } catch (
      ...) {}
  try {
    pobyt_okres = zaklUdaje.get_child("are:Pobyt").get_child("are:okres").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_obec = zaklUdaje.get_child("are:Pobyt").get_child("are:obec").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_castObce = zaklUdaje.get_child("are:Pobyt").get_child("are:castObce").get_value<std::string>();
  } catch (...) {}
  try { pobyt_mop = zaklUdaje.get_child("are:Pobyt").get_child("are:mop").get_value<std::string>(); } catch (
      ...) {}
  try {
    pobyt_ulice = zaklUdaje.get_child("are:Pobyt").get_child("are:ulice").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_cisloPop = zaklUdaje.get_child("are:Pobyt").get_child("are:cisloPop").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_cisloOr = zaklUdaje.get_child("are:Pobyt").get_child("are:cisloOr").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_cisloEv = zaklUdaje.get_child("are:Pobyt").get_child("are:cisloEv").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_cisloTxt = zaklUdaje.get_child("are:Pobyt").get_child("are:cisloTxt").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_doplnujiciText =
        zaklUdaje.get_child("are:Pobyt").get_child("are:doplnujiciText").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_dorucovaciCislo =
        zaklUdaje.get_child("are:Pobyt").get_child("are:dorucovaciCislo").get_value<std::string>();
  } catch (...) {}
  try {
    pobyt_skrytyUdaj =
        zaklUdaje.get_child("are:Pobyt").get_child("are:skrytyUdaj").get_child("are:text").get_value<std::string>();
  } catch (...) {}
  try { DatumVzniku = zaklUdaje.get_child("are:DatumVzniku").get_value<std::string>(); } catch (...) {}
  try { DatumZapisu = zaklUdaje.get_child("are:DatumZapisu").get_value<std::string>(); } catch (...) {}
  try { DatumVymazu = zaklUdaje.get_child("are:DatumVymazu").get_value<std::string>(); } catch (...) {}
  try { DuvodVymazu = zaklUdaje.get_child("are:DuvodVymazu").get_value<std::string>(); } catch (...) {}
  try {
    StatusVerejneProspesnosti = zaklUdaje.get_child("are:StatusVerejneProspesnosti").get_value<std::string>();
  } catch (...) {}
  try {
    PredmetPodnikani =
        zaklUdaje.get_child("are:Cinnosti").get_child("are:PredmetPodnikani").get_child("are:Text").get_value<
            std::string>();
  } catch (...) {}
  try {
    DoplnkovaCinnost =
        zaklUdaje.get_child("are:Cinnosti").get_child("are:DoplnkovaCinnost").get_child("are:Text").get_value<
            std::string>();
  } catch (...) {}
  try {
    PredmetCinnosti =
        zaklUdaje.get_child("are:Cinnosti").get_child("are:PredmetCinnosti").get_child("are:Text").get_value<
            std::string>();
  } catch (...) {}
  try {
    Ucel =
        zaklUdaje.get_child("are:Cinnosti").get_child("are:Ucel").get_child("are:Text").get_value<std::string>();
  } catch (...) {}
}

std::vector<std::string> ZakladniUdaje::toVector() {
  std::vector<std::string> result;
  result.emplace_back(ICO);
  result.emplace_back(ObchodniFirma);
  result.emplace_back(Kod_PF);
  result.emplace_back(Nazev_PF);
  result.emplace_back(sidlo_ruianKod);
  result.emplace_back(sidlo_stat);
  result.emplace_back(sidlo_psc);
  result.emplace_back(sidlo_okres);
  result.emplace_back(sidlo_obec);
  result.emplace_back(sidlo_castObce);
  result.emplace_back(sidlo_mop);
  result.emplace_back(sidlo_ulice);
  result.emplace_back(sidlo_cisloPop);
  result.emplace_back(sidlo_cisloOr);
  result.emplace_back(sidlo_cisloEv);
  result.emplace_back(sidlo_cisloTxt);
  result.emplace_back(sidlo_doplnujiciText);
  result.emplace_back(sidlo_dorucovaciCislo);
  result.emplace_back(sidlo_skrytyUdaj);
  result.emplace_back(bydliste_ruianKod);
  result.emplace_back(bydliste_stat);
  result.emplace_back(bydliste_psc);
  result.emplace_back(bydliste_okres);
  result.emplace_back(bydliste_obec);
  result.emplace_back(bydliste_castObce);
  result.emplace_back(bydliste_mop);
  result.emplace_back(bydliste_ulice);
  result.emplace_back(bydliste_cisloPop);
  result.emplace_back(bydliste_cisloOr);
  result.emplace_back(bydliste_cisloEv);
  result.emplace_back(bydliste_cisloTxt);
  result.emplace_back(bydliste_doplnujiciText);
  result.emplace_back(bydliste_dorucovaciCislo);
  result.emplace_back(bydliste_skrytyUdaj);
  result.emplace_back(pobyt_ruianKod);
  result.emplace_back(pobyt_stat);
  result.emplace_back(pobyt_psc);
  result.emplace_back(pobyt_okres);
  result.emplace_back(pobyt_obec);
  result.emplace_back(pobyt_castObce);
  result.emplace_back(pobyt_mop);
  result.emplace_back(pobyt_ulice);
  result.emplace_back(pobyt_cisloPop);
  result.emplace_back(pobyt_cisloOr);
  result.emplace_back(pobyt_cisloEv);
  result.emplace_back(pobyt_cisloTxt);
  result.emplace_back(pobyt_doplnujiciText);
  result.emplace_back(pobyt_dorucovaciCislo);
  result.emplace_back(pobyt_skrytyUdaj);
  result.emplace_back(DatumVzniku);
  result.emplace_back(DatumZapisu);
  result.emplace_back(DatumVymazu);
  result.emplace_back(DuvodVymazu);
  result.emplace_back(StatusVerejneProspesnosti);
  result.emplace_back(PredmetPodnikani);
  result.emplace_back(DoplnkovaCinnost);
  result.emplace_back(PredmetCinnosti);
  result.emplace_back(Ucel);
  return result;
}
std::vector<std::string> ZakladniUdaje::getHeader() {
  std::vector<std::string> result;
  result.emplace_back("ICO");
  result.emplace_back("ObchodniFirma");
  result.emplace_back("Kod_PF");
  result.emplace_back("Nazev_PF");
  result.emplace_back("sidlo_ruianKod");
  result.emplace_back("sidlo_stat");
  result.emplace_back("sidlo_psc");
  result.emplace_back("sidlo_okres");
  result.emplace_back("sidlo_obec");
  result.emplace_back("sidlo_castObce");
  result.emplace_back("sidlo_mop");
  result.emplace_back("sidlo_ulice");
  result.emplace_back("sidlo_cisloPop");
  result.emplace_back("sidlo_cisloOr");
  result.emplace_back("sidlo_cisloEv");
  result.emplace_back("sidlo_cisloTxt");
  result.emplace_back("sidlo_doplnujiciText");
  result.emplace_back("sidlo_dorucovaciCislo");
  result.emplace_back("sidlo_skrytyUdaj");
  result.emplace_back("bydliste_ruianKod");
  result.emplace_back("bydliste_stat");
  result.emplace_back("bydliste_psc");
  result.emplace_back("bydliste_okres");
  result.emplace_back("bydliste_obec");
  result.emplace_back("bydliste_castObce");
  result.emplace_back("bydliste_mop");
  result.emplace_back("bydliste_ulice");
  result.emplace_back("bydliste_cisloPop");
  result.emplace_back("bydliste_cisloOr");
  result.emplace_back("bydliste_cisloEv");
  result.emplace_back("bydliste_cisloTxt");
  result.emplace_back("bydliste_doplnujiciText");
  result.emplace_back("bydliste_dorucovaciCislo");
  result.emplace_back("bydliste_skrytyUdaj");
  result.emplace_back("pobyt_ruianKod");
  result.emplace_back("pobyt_stat");
  result.emplace_back("pobyt_psc");
  result.emplace_back("pobyt_okres");
  result.emplace_back("pobyt_obec");
  result.emplace_back("pobyt_castObce");
  result.emplace_back("pobyt_mop");
  result.emplace_back("pobyt_ulice");
  result.emplace_back("pobyt_cisloPop");
  result.emplace_back("pobyt_cisloOr");
  result.emplace_back("pobyt_cisloEv");
  result.emplace_back("pobyt_cisloTxt");
  result.emplace_back("pobyt_doplnujiciText");
  result.emplace_back("pobyt_dorucovaciCislo");
  result.emplace_back("pobyt_skrytyUdaj");
  result.emplace_back("DatumVzniku");
  result.emplace_back("DatumZapisu");
  result.emplace_back("DatumVymazu");
  result.emplace_back("DuvodVymazu");
  result.emplace_back("StatusVerejneProspesnosti");
  result.emplace_back("PredmetPodnikani");
  result.emplace_back("DoplnkovaCinnost");
  result.emplace_back("PredmetCinnosti");
  result.emplace_back("Ucel");
  return result;
}
