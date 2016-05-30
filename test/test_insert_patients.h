


void insert_patients(ext::ExtendedDatabase * db) {

  ext::Patient::pointer p;
  ext::Patient::vector patients;

  db->New(p);
  p->Set("toto", 1, 50,  "XAXYYZZ", "2002-08-09 10:00");
  patients.push_back(p);

  db->New(p);
  p->Set("titi", 2, 55, "AXXEYYZZ", "2005-02-01 17:00");
  patients.push_back(p);

  db->New(p);
  p->Set("titi2", 3, 70, "AXXRYYZZ", "2008-02-01 17:00");
  patients.push_back(p);

  db->New(p);
  p->Set("aaaa", 4, 77, "AXXYYEZZ", "2004-02-01 17:00");
  patients.push_back(p);

  db->New(p);
  p->Set("tututut", 5, 100, "AXRXYYZZ", "2015-02-01 17:00");
  patients.push_back(p);

  db->New(p);
  p->Set("okokok", 6, 46, "AXXYYRZZ", "2008-04-01 17:00");
  patients.push_back(p);

  db->New(p);
  p->Set("titi3", 7, 79, "unset", "2000-02-01 17:00");
  patients.push_back(p);

  db->New(p);
  p->Set("titi4", 10, 83, "unset", "1987-02-01 17:00");
  patients.push_back(p);

  db->Insert(patients);

  {
    syd::Radionuclide::pointer rad;
    db->New(rad);
    rad->name = "Indium111";
    rad->half_life_in_hours = 67.313;
    db->Insert(rad);
  }

  {
    syd::Radionuclide::pointer rad;
    db->New(rad);
    rad->name = "Yttrium90";
    rad->half_life_in_hours = 64.053;
    db->Insert(rad);
  }

  {
    syd::Tag::pointer tag;
    db->New(tag);
    tag->label = "debug";
    tag->description = "this is a debug tag";
    db->Insert(tag);
  }

  {
    syd::Tag::pointer tag;
    db->New(tag);
    tag->label = "bidule";
    tag->description = "this is another tag";
    db->Insert(tag);
  }

  {
    syd::Tag::pointer tag;
    db->New(tag);
    tag->label = "I_am_a_tag";
    tag->description = "this is yet another tag";
    db->Insert(tag);
  }



}
