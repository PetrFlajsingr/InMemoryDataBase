//
// Created by Petr Flajsingr on 2019-04-26.
//

#ifndef CSV_READER_ZAKLADNIUDAJE_H
#define CSV_READER_ZAKLADNIUDAJE_H

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

namespace pt = boost::property_tree;
struct ZakladniUdaje {
  ZakladniUdaje(const pt::ptree &ptree);

  std::string ICO;
  std::string ObchodniFirma;
  std::string Kod_PF;
  std::string Nazev_PF;
  std::string sidlo_ruianKod;
  std::string sidlo_stat;
  std::string sidlo_psc;
  std::string sidlo_okres;
  std::string sidlo_obec;
  std::string sidlo_castObce;
  std::string sidlo_mop;
  std::string sidlo_ulice;
  std::string sidlo_cisloPop;
  std::string sidlo_cisloOr;
  std::string sidlo_cisloEv;
  std::string sidlo_cisloTxt;
  std::string sidlo_doplnujiciText;
  std::string sidlo_dorucovaciCislo;
  std::string sidlo_skrytyUdaj;
  std::string bydliste_ruianKod;
  std::string bydliste_stat;
  std::string bydliste_psc;
  std::string bydliste_okres;
  std::string bydliste_obec;
  std::string bydliste_castObce;
  std::string bydliste_mop;
  std::string bydliste_ulice;
  std::string bydliste_cisloPop;
  std::string bydliste_cisloOr;
  std::string bydliste_cisloEv;
  std::string bydliste_cisloTxt;
  std::string bydliste_doplnujiciText;
  std::string bydliste_dorucovaciCislo;
  std::string bydliste_skrytyUdaj;
  std::string pobyt_ruianKod;
  std::string pobyt_stat;
  std::string pobyt_psc;
  std::string pobyt_okres;
  std::string pobyt_obec;
  std::string pobyt_castObce;
  std::string pobyt_mop;
  std::string pobyt_ulice;
  std::string pobyt_cisloPop;
  std::string pobyt_cisloOr;
  std::string pobyt_cisloEv;
  std::string pobyt_cisloTxt;
  std::string pobyt_doplnujiciText;
  std::string pobyt_dorucovaciCislo;
  std::string pobyt_skrytyUdaj;
  std::string DatumVzniku;
  std::string DatumZapisu;
  std::string DatumVymazu;
  std::string DuvodVymazu;
  std::string StatusVerejneProspesnosti;
  std::string PredmetPodnikani;
  std::string DoplnkovaCinnost;
  std::string PredmetCinnosti;
  std::string Ucel;

  std::vector<std::string> getHeader();

  std::vector<std::string> toVector();
};

#endif // CSV_READER_ZAKLADNIUDAJE_H
