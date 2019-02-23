//
// Created by Petr Flajsingr on 2019-02-10.
//

#include <MemoryDataSet.h>
#include <LexicalAnalyser.h>
#include <SyntaxAnalyser.h>
#include <SemanticAnalyser.h>
#include <MemoryDataBase.h>
#include <JoinMaker.h>
#include <CsvReader.h>
#include <XlsxWriter.h>

void terminate_handler() {
  try {
    auto unknown = std::current_exception();
    if (unknown) {
      std::rethrow_exception(unknown);
    } else {
      std::cerr << "normal termination" << std::endl;
    }
  } catch (const std::exception &e) {
    std::cerr << "Exc type: " << typeid(e).name() << "\nExc.what: " << e.what()
              << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception of type" << std::endl;
  }
  abort();
}

void agrTest() {
  const std::string file1 = "/Users/petr/Desktop/agr test/test.csv";
  const std::string file2 = "/Users/petr/Desktop/agr test/test2.csv";
  const std::string outFile = "/Users/petr/Desktop/agr_test_out.csv";
  auto prov1 = DataProviders::CsvReader(file1);
  auto prov2 = DataProviders::CsvReader(file2);
  auto ds1 = std::make_shared<DataSets::MemoryDataSet>("t1");
  ds1->open(prov1,
            {ValueType::Integer, ValueType::Currency, ValueType::Integer,
             ValueType::Integer});
  auto ds2 = std::make_shared<DataSets::MemoryDataSet>("t2");
  ds2->open(prov2,
            {ValueType::Integer, ValueType::Currency, ValueType::Integer,
             ValueType::Integer});

  DataBase::MemoryDataBase db("testDB");
  db.addTable(ds1);
  db.addTable(ds2);

  const std::string
      query = "select t1.group1, t1.group2, sum(t1.suma)"
              " from t1 "
              "group by t1.group1, t1.group2;";

  auto view = db.execAggregateQuery(query, "tmpView");

  auto writer = DataWriters::CsvWriter(outFile);

  writer.writeHeader(view->dataSet->getFieldNames());

  auto fields = view->dataSet->getFields();

  while (view->dataSet->next()) {
    std::vector<std::string> record;
    std::transform(fields.begin(),
                   fields.end(),
                   std::back_inserter(record),
                   [](const DataSets::BaseField *field) {
                     return std::string(field->getAsString());
                   });
    writer.writeRecord(record);
  }
}

int main() {
  agrTest();
  return 0;
  //std::set_terminate(terminate_handler);

  /*const std::string registr = "/Users/petr/Desktop/registr.csv";
  const std::string nno = "/Users/petr/Desktop/csvs/NNO_subjekty6ver2.4.csv";

  auto prov1 = DataProviders::CsvReader(registr, ",");
  auto prov2 = DataProviders::CsvReader(nno, ";");

  auto ds1 = std::make_shared<DataSets::MemoryDataSet>("registr");
  ds1->open(prov1, {ValueType::Integer,
                    ValueType::Integer,
                    ValueType::Integer,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::Integer,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::Integer,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String});
  auto ds2 = std::make_shared<DataSets::MemoryDataSet>("nno");
  ds2->open(prov2, {ValueType::Integer,
                    ValueType::Integer,
                    ValueType::Integer,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String});

  DataBase::MemoryDataBase db("testDB");
  db.addTable(ds1);
  db.addTable(ds2);

  const std::string
      query = "select nno.*, registr.id, registr.pcz, registr.pcdp, "
              "registr.nazev, registr.typ, registr.adresa_nazev_obce, "
              "registr.adresa_psc, registr.adresa_nazev_ulice, "
              "registr.adresa_cislo_domovni, registr.adresa_nazev_kraje, "
              "registr.adresa_kod_kraje, registr.adresa_nazev_okresu, "
              "registr.adresa_kod_okresu, registr.spravni_obvod, registr.nazev_zkraceny, "
              "registr.poskytovatel_ic, registr.poskytovatel_pravni_forma_osoba, "
              "registr.poskytovatel_pravni_forma, registr.sidlo_adresa_kod_kraje, "
              "registr.sidlo_adresa_nazev_kraje, registr.sidlo_adresa_kod_okresu, "
              "registr.sidlo_adresa_nazev_okresu, registr.sidlo_adresa_psc, "
              "registr.sidlo_adresa_nazev_obce, registr.sidlo_adresa_nazev_ulice, "
              "registr.sidlo_adresa_cislo_domovni, registr.obor_pece, "
              "registr.forma_pece, registr.druh_pece, registr.poskytovatel_souradnice "
              "from nno "
              "join registr on nno.ICO_num = registr.poskytovatel_ic;";

  auto view = db.execSimpleQuery(query, false, "wat");

  auto writer = DataWriters::XlsxWriter("/Users/petr/Desktop/wow.xlsx");
  writer.writeHeader(view->dataSet->getFieldNames());
  auto fields = view->dataSet->getFields();
  while (view->dataSet->next()) {
    std::vector<std::string> record;
    std::transform(fields.begin(),
                   fields.end(),
                   std::back_inserter(record),
                   [](const DataSets::BaseField *field) {
                     return std::string(field->getAsString());
                   });
    writer.writeRecord(record);
  }

  return 0;*/


  /*const std::string file1 = "/Users/petr/Desktop/join_test_1.csv";
  const std::string file2 = "/Users/petr/Desktop/join_test_2.csv";
  const std::string outFile = "/Users/petr/Desktop/join_test_out.csv";
  auto prov1 = DataProviders::CsvReader(file1);
  auto prov2 = DataProviders::CsvReader(file2);
  auto ds1 = std::make_shared<DataSets::MemoryDataSet>("test1");
  ds1->open(prov1, {ValueType::String, ValueType::String, ValueType::Integer});
  auto ds2 = std::make_shared<DataSets::MemoryDataSet>("test2");
  ds2->open(prov2, {ValueType::String, ValueType::String, ValueType::Integer});

  DataBase::MemoryDataBase db("testDB");
  db.addTable(ds1);
  db.addTable(ds2);

  const std::string
      query = "select test1.*, test2.B, test2.C "
              "from test1 join test2 on test1.A1 = test2.A left join test2 on test1.A1 = test2.A "
              //"where test1.A1 = \"A\" | \"B\" and test1.C1 = 11 | 22 | 33 | 44 and test2.A = \"A\" "
              "order by test2.A asc, test2.C desc;";

  auto view = db.execSimpleQuery(query, false, "tmpView");

  auto writer = DataWriters::CsvWriter(outFile);

  writer.writeHeader(view->dataSet->getFieldNames());

  auto fields = view->dataSet->getFields();

  while (view->dataSet->next()) {
    std::vector<std::string> record;
    std::transform(fields.begin(),
                   fields.end(),
                   std::back_inserter(record),
                   [](const DataSets::BaseField *field) {
                     return std::string(field->getAsString());
                   });
    writer.writeRecord(record);
  }

  return 0;*/

}