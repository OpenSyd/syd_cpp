


void insert_injections(ext::ExtendedDatabase * db) {

  syd::Injection::pointer inj;
  syd::Injection::vector injections;

  db->New(inj);
  std::vector<std::string> args;
  args.push_back("toto");
  args.push_back("Indium111");
  args.push_back("2002-08-09 09:00");
  args.push_back("180.2");
  inj->Set(args);
  injections.push_back(inj);

  db->New(inj);
  args[0] = "toto";
  args[1] = "Yttrium90";
  args[2] = "2002-09-09 09:00";
  args[3] = "280.2";
  inj->Set(args);
  injections.push_back(inj);

  db->New(inj);
  args[0] = "titi";
  args[1] = "Indium111";
  args[2] = "2012-09-09 09:00";
  args[3] = "80.2";
  inj->Set(args);
  injections.push_back(inj);

  db->New(inj);
  args[0] = "titi2";
  args[1] = "Indium111";
  args[2] = "2002-09-19 09:00";
  args[3] = "1280.2";
  inj->Set(args);
  injections.push_back(inj);

  db->Insert(injections);

}
