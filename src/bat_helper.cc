/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat_helper.h"

#include <sstream>
#include <random>
#include <utility>
#include <iomanip>
#include <ctime>
#include <memory>
#include <iostream>

#include <openssl/base64.h>
#include <openssl/digest.h>
#include <openssl/hkdf.h>
#include <openssl/sha.h>

#include "rapidjson_bat_helper.h"
#include "static_values.h"
#include "tweetnacl.h"

namespace braveledger_bat_helper {

  REQUEST_CREDENTIALS_ST::REQUEST_CREDENTIALS_ST() {}

  REQUEST_CREDENTIALS_ST::~REQUEST_CREDENTIALS_ST() {}

  /////////////////////////////////////////////////////////////////////////////
  RECONCILE_PAYLOAD_ST::RECONCILE_PAYLOAD_ST() {}

  RECONCILE_PAYLOAD_ST::~RECONCILE_PAYLOAD_ST() {}

  /////////////////////////////////////////////////////////////////////////////
  WALLET_INFO_ST::WALLET_INFO_ST() {}

  WALLET_INFO_ST::WALLET_INFO_ST(const WALLET_INFO_ST& other) {
    paymentId_ = other.paymentId_;
    addressBAT_ = other.addressBAT_;
    addressBTC_ = other.addressBTC_;
    addressCARD_ID_ = other.addressCARD_ID_;
    addressETH_ = other.addressETH_;
    addressLTC_ = other.addressLTC_;
    keyInfoSeed_ = other.keyInfoSeed_;
  }

  WALLET_INFO_ST::~WALLET_INFO_ST() {}


  bool WALLET_INFO_ST::loadFromJson(const std::string & json)
  {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !( d["paymentId"].IsString() &&
        d["addressBAT"].IsString() &&
        d["addressBTC"].IsString() &&
        d["addressCARD_ID"].IsString() &&
        d["addressETH"].IsString() &&
        d["addressLTC"].IsString() &&
        d["keyInfoSeed"].IsString() );
    }

    //convert keyInfoSeed and check error
    std::string sKeyInfoSeed = d["keyInfoSeed"].GetString();
    error = ! getFromBase64(sKeyInfoSeed, keyInfoSeed_);

    if (false == error)
    {
      paymentId_ = d["paymentId"].GetString();
      addressBAT_ = d["addressBAT"].GetString();
      addressBTC_ = d["addressBTC"].GetString();
      addressCARD_ID_ = d["addressCARD_ID"].GetString();
      addressETH_ = d["addressETH"].GetString();
      addressLTC_ = d["addressLTC"].GetString();
    }
    return !error;
  }

  void saveToJson(JsonWriter & writer, const WALLET_INFO_ST& data)
  {
    writer.StartObject();

    writer.String("paymentId");
    writer.String(data.paymentId_.c_str());

    writer.String("addressBAT");
    writer.String(data.addressBAT_.c_str());

    writer.String("addressBTC");
    writer.String(data.addressBTC_.c_str());

    writer.String("addressCARD_ID");
    writer.String(data.addressCARD_ID_.c_str());

    writer.String("addressETH");
    writer.String(data.addressETH_.c_str());

    writer.String("addressLTC");
    writer.String(data.addressLTC_.c_str());

    writer.String("keyInfoSeed");
    writer.String(getBase64(data.keyInfoSeed_).c_str());

    writer.EndObject();
  }

  /////////////////////////////////////////////////////////////////////////////
  UNSIGNED_TX::UNSIGNED_TX() {}

  UNSIGNED_TX::~UNSIGNED_TX() {}

  /////////////////////////////////////////////////////////////////////////////
  TRANSACTION_BALLOT_ST::TRANSACTION_BALLOT_ST() :
    offset_(0) {}

  TRANSACTION_BALLOT_ST::~TRANSACTION_BALLOT_ST() {}

  bool TRANSACTION_BALLOT_ST::loadFromJson(const std::string & json)
  {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !( d["publisher"].IsString() &&
        d["offset"].IsUint() );
    }

    if (false == error)
    {
      publisher_ = d["publisher"].GetString();
      offset_ = d["offset"].GetUint();
    }

    return !error;
  }

  void saveToJson(JsonWriter & writer, const TRANSACTION_BALLOT_ST& data)
  {
    writer.StartObject();

    writer.String("publisher");
    writer.String(data.publisher_.c_str());

    writer.String("offset");
    writer.Uint(data.offset_);

    writer.EndObject();
  }

  /////////////////////////////////////////////////////////////////////////////
  TRANSACTION_ST::TRANSACTION_ST():
    votes_(0) {}

  TRANSACTION_ST::TRANSACTION_ST(const TRANSACTION_ST& transaction) {
    viewingId_ = transaction.viewingId_;
    surveyorId_ = transaction.surveyorId_;
    contribution_fiat_amount_ = transaction.contribution_fiat_amount_;
    contribution_fiat_currency_ = transaction.contribution_fiat_currency_;
    contribution_rates_ = transaction.contribution_rates_;
    contribution_altcurrency_ = transaction.contribution_altcurrency_;
    contribution_probi_ = transaction.contribution_probi_;
    contribution_fee_ = transaction.contribution_fee_;
    submissionStamp_ = transaction.submissionStamp_;
    submissionId_ = transaction.submissionId_;
    contribution_rates_ = transaction.contribution_rates_;
    anonizeViewingId_ = transaction.anonizeViewingId_;
    registrarVK_ = transaction.registrarVK_;
    masterUserToken_ = transaction.masterUserToken_;
    surveyorIds_ = transaction.surveyorIds_;
    satoshis_ = transaction.satoshis_;
    altCurrency_ = transaction.altCurrency_;
    probi_ = transaction.probi_;
    votes_ = transaction.votes_;
    ballots_ = transaction.ballots_;
  }

  TRANSACTION_ST::~TRANSACTION_ST() {}

  bool TRANSACTION_ST::loadFromJson(const std::string & json)
  {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["viewingId"].IsString() &&
        d["surveyorId"].IsString() &&
        d["contribution_fiat_amount"].IsString() &&
        d["contribution_fiat_currency"].IsString() &&
        d["rates"].IsObject() &&
        d["rates"].HasMember("ETH") &&
        d["rates"].HasMember("LTC") &&
        d["rates"].HasMember("BTC") &&
        d["rates"].HasMember("USD") &&
        d["rates"].HasMember("EUR") &&
        d["contribution_altcurrency"].IsString() &&
        d["contribution_probi"].IsString() &&
        d["contribution_fee"].IsString() &&
        d["submissionStamp"].IsString() &&
        d["submissionId"].IsString() &&
        d["anonizeViewingId"].IsString() &&
        d["registrarVK"].IsString() &&
        d["masterUserToken"].IsString() &&
        d["surveyorIds"].IsArray() &&
        d["satoshis"].IsString() &&
        d["altCurrency"].IsString() &&
        d["probi"].IsString() &&
        d["votes"].IsUint() &&
        d["ballots"].IsArray());
    }

    if (false == error)
    {
      viewingId_ = d["viewingId"].GetString();
      surveyorId_ = d["surveyorId"].GetString();
      contribution_fiat_amount_ = d["contribution_fiat_amount"].GetString();
      contribution_fiat_currency_ = d["contribution_fiat_currency"].GetString();
      contribution_altcurrency_ = d["contribution_altcurrency"].GetString();
      contribution_probi_ = d["contribution_probi"].GetString();
      contribution_fee_ = d["contribution_fee"].GetString();
      submissionStamp_ = d["submissionStamp"].GetString();
      submissionId_ = d["submissionId"].GetString();
      anonizeViewingId_ = d["anonizeViewingId"].GetString();
      registrarVK_ = d["registrarVK"].GetString();
      masterUserToken_ = d["masterUserToken"].GetString();
      satoshis_ = d["satoshis"].GetString();
      altCurrency_ = d["altCurrency"].GetString();
      probi_ = d["probi"].GetString();
      votes_ = d["votes"].GetUint();

      for ( auto & i : d["rates"].GetObject())
      {
        contribution_rates_.insert(std::make_pair(i.name.GetString(), i.value.GetDouble() ));
      }

      for (auto & i : d["surveyorIds"].GetArray())
      {
        surveyorIds_.push_back(i.GetString());
      }

      for (const auto & i : d["ballots"].GetArray() )
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        i.Accept(writer);

        TRANSACTION_BALLOT_ST ballot;
        ballot.loadFromJson(sb.GetString());
        ballots_.push_back(ballot);
      }
    }

    return !error;
  }


  void saveToJson(JsonWriter & writer, const TRANSACTION_ST& data)
  {
    writer.StartObject();

    writer.String("viewingId");
    writer.String(data.viewingId_.c_str());

    writer.String("surveyorId");
    writer.String(data.surveyorId_.c_str());

    writer.String("contribution_fiat_amount");
    writer.String(data.contribution_fiat_amount_.c_str());

    writer.String("contribution_fiat_currency");
    writer.String(data.contribution_fiat_currency_.c_str());

    writer.String("rates");
    writer.StartObject();
    for (auto & p : data.contribution_rates_)
    {
      writer.String(p.first.c_str());
      writer.Double(p.second);
    }
    writer.EndObject();

    writer.String("contribution_altcurrency");
    writer.String(data.contribution_altcurrency_.c_str());

    writer.String("contribution_probi");
    writer.String(data.contribution_probi_.c_str());

    writer.String("contribution_fee");
    writer.String(data.contribution_fee_.c_str());

    writer.String("submissionStamp");
    writer.String(data.submissionStamp_.c_str());

    writer.String("submissionId");
    writer.String(data.submissionId_.c_str());

    writer.String("anonizeViewingId");
    writer.String(data.anonizeViewingId_.c_str());

    writer.String("registrarVK");
    writer.String(data.registrarVK_.c_str());

    writer.String("masterUserToken");
    writer.String(data.masterUserToken_.c_str());

    writer.String("surveyorIds");
    writer.StartArray();
    for (auto & i : data.surveyorIds_)
    {
      writer.String(i.c_str());
    }
    writer.EndArray();

    writer.String("satoshis");
    writer.String(data.satoshis_.c_str());

    writer.String("altCurrency");
    writer.String(data.altCurrency_.c_str());

    writer.String("probi");
    writer.String(data.probi_.c_str());

    writer.String("votes");
    writer.Uint(data.votes_);

    /* TODO: clarify if it needs to be serialized
    writer.String("ballots");
    writer.StartArray();
    for (auto & i : data.ballots_)
    {
      saveToJson(writer, i);
    }
    writer.EndArray();
    */

    writer.EndObject();
  }

  /////////////////////////////////////////////////////////////////////////////
  BALLOT_ST::BALLOT_ST():
    offset_(0),
    delayStamp_(0) {}

  BALLOT_ST::BALLOT_ST(const BALLOT_ST& ballot) {
    viewingId_ = ballot.viewingId_;
    surveyorId_ = ballot.surveyorId_;
    publisher_ = ballot.publisher_;
    offset_ = ballot.offset_;
    prepareBallot_ = ballot.prepareBallot_;
    proofBallot_ = ballot.proofBallot_;
    delayStamp_ = ballot.delayStamp_;
  }

  BALLOT_ST::~BALLOT_ST() {}

  bool BALLOT_ST::loadFromJson(const std::string & json)
  {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["viewingId"].IsString() &&
        d["surveyorId"].IsString() &&
        d["publisher"].IsString() &&
        d["offset"].IsUint() &&
        d["prepareBallot"].IsString() &&
        d["delayStamp"].IsUint64() );
    }

    if (false == error)
    {
      viewingId_ = d["viewingId"].GetString();
      surveyorId_ = d["surveyorId"].GetString();
      publisher_ = d["publisher"].GetString();
      offset_ = d["offset"].GetUint();
      prepareBallot_ = d["prepareBallot"].GetString();
      delayStamp_ = d["delayStamp"].GetUint64();
    }

    return !error;
  }

  void saveToJson(JsonWriter & writer, const BALLOT_ST& data)
  {
    writer.StartObject();

    writer.String("viewingId");
    writer.String(data.viewingId_.c_str());

    writer.String("surveyorId");
    writer.String(data.surveyorId_.c_str());

    writer.String("publisher");
    writer.String(data.publisher_.c_str());

    writer.String("offset");
    writer.Uint(data.offset_);

    writer.String("prepareBallot");
    writer.String(data.prepareBallot_.c_str());

    writer.String("delayStamp");
    writer.Uint64(data.delayStamp_);

    writer.EndObject();
  }

  /////////////////////////////////////////////////////////////////////////////
  CLIENT_STATE_ST::CLIENT_STATE_ST():
    bootStamp_(0),
    reconcileStamp_(0),
    settings_(AD_FREE_SETTINGS),
    fee_amount_(0),
    days_(0)  {}

  CLIENT_STATE_ST::CLIENT_STATE_ST(const CLIENT_STATE_ST& other) {
    walletInfo_ = other.walletInfo_;
    bootStamp_ = other.bootStamp_;
    reconcileStamp_ = other.reconcileStamp_;
    personaId_ = other.personaId_;
    userId_ = other.userId_;
    registrarVK_ = other.registrarVK_;
    masterUserToken_ = other.masterUserToken_;
    preFlight_ = other.preFlight_;
    fee_currency_ = other.fee_currency_;
    settings_ = other.settings_;
    fee_amount_ = other.fee_amount_;
    days_ = other.days_;
    transactions_ = other.transactions_;
    ballots_ = other.ballots_;
    ruleset_ = other.ruleset_;
    rulesetV2_ = other.rulesetV2_;
  }

  CLIENT_STATE_ST::~CLIENT_STATE_ST() {}

  bool CLIENT_STATE_ST::loadFromJson(const std::string & json)
  {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser error or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["walletInfo"].IsObject() &&
        d["bootStamp"].IsUint64() &&
        d["reconcileStamp"].IsUint64() &&
        d["personaId"].IsString() &&
        d["userId"].IsString() &&
        d["registrarVK"].IsString() &&
        d["masterUserToken"].IsString() &&
        d["preFlight"].IsString() &&
        d["fee_currency"].IsString() &&
        d["settings"].IsString() &&
        d["fee_amount"].IsDouble() &&
        d["days"].IsUint() &&
        d["transactions"].IsArray() &&
        d["ballots"].IsArray() );
    }

    if (false == error)
    {
      {
        auto & i = d["walletInfo"];
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        i.Accept(writer);
        walletInfo_.loadFromJson(sb.GetString());
      }

      bootStamp_ = d["bootStamp"].GetUint64();
      reconcileStamp_ = d["reconcileStamp"].GetUint64();
      personaId_ = d["personaId"].GetString();
      userId_ = d["userId"].GetString();
      registrarVK_ = d["registrarVK"].GetString();
      masterUserToken_ = d["masterUserToken"].GetString();
      preFlight_ = d["preFlight"].GetString();
      fee_currency_ = d["fee_currency"].GetString();
      settings_ = d["settings"].GetString();
      fee_amount_ = d["fee_amount"].GetDouble();
      days_ = d["days"].GetUint();

      for (const auto & i : d["transactions"].GetArray())
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        i.Accept(writer);

        TRANSACTION_ST ta;
        ta.loadFromJson(sb.GetString());
        transactions_.push_back(ta);
      }

      for (const auto & i : d["ballots"].GetArray())
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        i.Accept(writer);

        BALLOT_ST b;
        b.loadFromJson(sb.GetString());
        ballots_.push_back(b);
      }
    }

    return !error;
  }

  void saveToJson(JsonWriter & writer, const CLIENT_STATE_ST& data)
  {
    writer.StartObject();

    writer.String("walletInfo");
    saveToJson(writer, data.walletInfo_);

    writer.String("bootStamp");
    writer.Uint64(data.bootStamp_);

    writer.String("reconcileStamp");
    writer.Uint64(data.reconcileStamp_);

    writer.String("personaId");
    writer.String(data.personaId_.c_str());

    writer.String("userId");
    writer.String(data.userId_.c_str());

    writer.String("registrarVK");
    writer.String(data.registrarVK_.c_str());

    writer.String("masterUserToken");
    writer.String(data.masterUserToken_.c_str());

    writer.String("preFlight");
    writer.String(data.preFlight_.c_str());

    writer.String("fee_currency");
    writer.String(data.fee_currency_.c_str());

    writer.String("settings");
    writer.String(data.settings_.c_str());

    writer.String("fee_amount");
    writer.Double(data.fee_amount_);

    writer.String("days");
    writer.Uint(data.days_);

    writer.String("transactions");
    writer.StartArray();
    for (auto & t : data.transactions_)
    {
      saveToJson(writer, t);
    }
    writer.EndArray();

    writer.String("ballots");
    writer.StartArray();
    for (auto & b : data.ballots_)
    {
      saveToJson(writer, b);
    }
    writer.EndArray();

    writer.String("ruleset");
    writer.String(data.ruleset_.c_str());

    writer.String("rulesetV2");
    writer.String(data.rulesetV2_.c_str());

    writer.EndObject();
  }

  /////////////////////////////////////////////////////////////////////////////
  PUBLISHER_STATE_ST::PUBLISHER_STATE_ST():
    min_pubslisher_duration_(braveledger_ledger::_default_min_pubslisher_duration),
    min_visits_(1),
    allow_non_verified_(true) {}

  PUBLISHER_STATE_ST::~PUBLISHER_STATE_ST() {}

  bool PUBLISHER_STATE_ST::loadFromJson(const std::string & json)
  {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["min_pubslisher_duration"].IsUint() &&
        d["min_visits"].IsUint() &&
        d["allow_non_verified"].IsBool());
    }

    if (false == error)
    {
      min_pubslisher_duration_ = d["min_pubslisher_duration"].GetUint();
      min_visits_ = d["min_visits"].GetUint();
      allow_non_verified_ = d["allow_non_verified"].GetBool();
    }

    return !error;
  }

  void saveToJson(JsonWriter & writer, const PUBLISHER_STATE_ST& data)
  {
    writer.StartObject();

    writer.String("min_pubslisher_duration");
    writer.Uint(data.min_pubslisher_duration_);

    writer.String("min_visits");
    writer.Uint(data.min_visits_);

    writer.String("allow_non_verified");
    writer.Bool(data.allow_non_verified_);

    writer.EndObject();
  }

  /////////////////////////////////////////////////////////////////////////////
  PUBLISHER_ST::PUBLISHER_ST():
    duration_(0),
    score_(0),
    visits_(0),
    verified_(false),
    exclude_(false),
    pinPercentage_(false),
    verifiedTimeStamp_(0),
    percent_(0),
    deleted_(false),
    weight_(0) {}

  PUBLISHER_ST::PUBLISHER_ST(const PUBLISHER_ST& publisher) :
    duration_(publisher.duration_),
    favicon_url_(publisher.favicon_url_),
    score_(publisher.score_),
    visits_(publisher.visits_),
    verified_(publisher.verified_),
    exclude_(publisher.exclude_),
    pinPercentage_(publisher.pinPercentage_),
    verifiedTimeStamp_(publisher.verifiedTimeStamp_),
    percent_(publisher.percent_),
    deleted_(publisher.deleted_),
    weight_(publisher.weight_)
    {}

  PUBLISHER_ST::~PUBLISHER_ST() {}

  bool PUBLISHER_ST::loadFromJson(const std::string & json)
  {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["duration"].IsUint64() &&
        d["favicon_url"].IsString() &&
        d["score"].IsDouble() &&
        d["visits"].IsUint() &&
        d["verified"].IsBool() &&
        d["exclude"].IsBool() &&
        d["pinPercentage"].IsBool() &&
        d["verifiedTimeStamp"].IsUint64() &&
        d["percent"].IsUint() &&
        d["deleted"].IsBool() &&
        d["weight"].IsDouble() );
    }

    if (false == error)
    {
      //TODO: Uint64 or string?
      duration_ = d["duration"].GetUint64();
      favicon_url_ = d["favicon_url"].GetString();
      score_ = d["score"].GetDouble();
      visits_ = d["visits"].GetUint();
      verified_ = d["verified"].GetBool();
      exclude_ = d["exclude"].GetBool();
      pinPercentage_ = d["pinPercentage"].GetBool();
      verifiedTimeStamp_ = d["verifiedTimeStamp"].GetUint64();
      percent_ = d["percent"].GetUint();
      deleted_ = d["deleted"].GetBool();
      weight_ = d["weight"].GetDouble();
    }

    return !error;
  }

  void saveToJson(JsonWriter & writer, const PUBLISHER_ST& data)
  {
    writer.StartObject();

    //TODO: uint64 or string
    writer.String("duration");
    writer.Uint64(data.duration_);

    writer.String("favicon_url");
    writer.String(data.favicon_url_.c_str());

    writer.String("score");
    writer.Double(data.score_);

    writer.String("visits");
    writer.Uint(data.visits_);

    writer.String("verified");
    writer.Bool(data.verified_);

    writer.String("exclude");
    writer.Bool(data.exclude_);

    writer.String("pinPercentage");
    writer.Bool(data.pinPercentage_);

    //TODO: uint64 or string
    writer.String("verifiedTimeStamp");
    writer.Uint64(data.verifiedTimeStamp_);

    writer.String("percent");
    writer.Uint(data.percent_);

    writer.String("deleted");
    writer.Bool(data.deleted_);

    writer.String("weight");
    writer.Double(data.weight_);

    writer.EndObject();
  }

  /////////////////////////////////////////////////////////////////////////////
  PUBLISHER_DATA_ST::PUBLISHER_DATA_ST() :
    daysSpent_(0),
    hoursSpent_(0),
    minutesSpent_(0),
    secondsSpent_(0) {}

  PUBLISHER_DATA_ST::PUBLISHER_DATA_ST(const PUBLISHER_DATA_ST& publisherData) :
    publisherKey_(publisherData.publisherKey_),
    publisher_(publisherData.publisher_),
    daysSpent_(publisherData.daysSpent_),
    hoursSpent_(publisherData.hoursSpent_),
    minutesSpent_(publisherData.minutesSpent_),
    secondsSpent_(publisherData.secondsSpent_) {}

  PUBLISHER_DATA_ST::~PUBLISHER_DATA_ST() {}

  bool PUBLISHER_DATA_ST::operator<(const PUBLISHER_DATA_ST &rhs) const {
    return publisher_.score_ > rhs.publisher_.score_;
  }

  /////////////////////////////////////////////////////////////////////////////
  WINNERS_ST::WINNERS_ST() :
    votes_(0) {}

  WINNERS_ST::~WINNERS_ST() {}

  /////////////////////////////////////////////////////////////////////////////
  WALLET_PROPERTIES_ST::WALLET_PROPERTIES_ST() {}

  WALLET_PROPERTIES_ST::~WALLET_PROPERTIES_ST() {}

  bool WALLET_PROPERTIES_ST::loadFromJson(const std::string & json)
  {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["altcurrency"].IsString() &&
        d["balance"].IsDouble() &&
        d["rates"].IsObject() &&
        d["parameters"].IsObject() );
    }

    if (false == error)
    {
      altcurrency_ = d["altcurrency"].GetString();
      balance_ = d["balance"].GetDouble();

      for (auto & i : d["rates"].GetObject())
      {
        rates_.insert(std::make_pair(i.name.GetString(), i.value.GetDouble()));
      }

      parameters_currency_ = d["parameters"]["adFree"]["currency"].GetString();
      parameters_fee_ = d["parameters"]["adFree"]["fee"]["BAT"].GetDouble();

      for (auto & i : d["parameters"]["adFree"]["choices"]["BAT"].GetArray())
      {
        parameters_choices_.push_back(i.GetDouble());
      }

      for (auto & i : d["parameters"]["adFree"]["range"]["BAT"].GetArray())
      {
        parameters_range_.push_back(i.GetDouble());
      }

      parameters_days_ = d["parameters"]["adFree"]["days"].GetUint();
    }
    return !error;
  }

  /////////////////////////////////////////////////////////////////////////////
  FETCH_CALLBACK_EXTRA_DATA_ST::FETCH_CALLBACK_EXTRA_DATA_ST():
    value1(0),
    boolean1(true) {}

  FETCH_CALLBACK_EXTRA_DATA_ST::FETCH_CALLBACK_EXTRA_DATA_ST(const FETCH_CALLBACK_EXTRA_DATA_ST& extraData):
    value1(extraData.value1),
    string1(extraData.string1),
    string2(extraData.string2),
    string3(extraData.string3),
    string4(extraData.string4),
    string5(extraData.string5),
    boolean1(extraData.boolean1) {}

  FETCH_CALLBACK_EXTRA_DATA_ST::~FETCH_CALLBACK_EXTRA_DATA_ST() {}

  /////////////////////////////////////////////////////////////////////////////
  SURVEYOR_INFO_ST::SURVEYOR_INFO_ST() {}

  SURVEYOR_INFO_ST::~SURVEYOR_INFO_ST() {}

 /////////////////////////////////////////////////////////////////////////////
  CURRENT_RECONCILE::CURRENT_RECONCILE() :
    timestamp_(0) {}

  CURRENT_RECONCILE::~CURRENT_RECONCILE() {}

  /////////////////////////////////////////////////////////////////////////////
  SURVEYOR_ST::SURVEYOR_ST() {}

  SURVEYOR_ST::~SURVEYOR_ST() {}

  bool SURVEYOR_ST::loadFromJson(const std::string & json)
  {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["signature"].IsString() &&
        d["surveyorId"].IsString() &&
        d["surveyVK"].IsString() &&
        d["registrarVK"].IsString() &&
        d["surveySK"].IsString() );
    }

    if (false == error)
    {
      signature_ = d["signature"].GetString();
      surveyorId_ = d["surveyorId"].GetString();
      surveyVK_ = d["surveyVK"].GetString();
      registrarVK_ = d["registrarVK"].GetString();
      surveySK_ = d["surveySK"].GetString();
    }

    return !error;
  }

  void saveToJson(JsonWriter & writer, const SURVEYOR_ST& data)
  {
    writer.StartObject();

    writer.String("signature");
    writer.String(data.signature_.c_str());

    writer.String("surveyorId");
    writer.String(data.surveyorId_.c_str());

    writer.String("surveyVK");
    writer.String(data.surveyVK_.c_str());

    writer.String("registrarVK");
    writer.String(data.registrarVK_.c_str());

    writer.String("surveySK");
    writer.String(data.surveySK_.c_str());

    writer.EndObject();
  }

  /////////////////////////////////////////////////////////////////////////////
  TWITCH_EVENT_INFO::TWITCH_EVENT_INFO() {}

  TWITCH_EVENT_INFO::TWITCH_EVENT_INFO(const TWITCH_EVENT_INFO& twitchEventInfo):
    event_(twitchEventInfo.event_),
    time_(twitchEventInfo.time_),
    status_(twitchEventInfo.status_) {}

  TWITCH_EVENT_INFO::~TWITCH_EVENT_INFO() {}

/////////////////////////////////////////////////////////////////////////////
  MEDIA_PUBLISHER_INFO::MEDIA_PUBLISHER_INFO() {}

  MEDIA_PUBLISHER_INFO::MEDIA_PUBLISHER_INFO(const MEDIA_PUBLISHER_INFO& mediaPublisherInfo):
    publisherName_(mediaPublisherInfo.publisherName_),
    publisherURL_(mediaPublisherInfo.publisherURL_),
    favIconURL_(mediaPublisherInfo.favIconURL_),
    channelName_(mediaPublisherInfo.channelName_),
    publisher_(mediaPublisherInfo.publisher_),
    twitchEventInfo_(mediaPublisherInfo.twitchEventInfo_) {}

  MEDIA_PUBLISHER_INFO::~MEDIA_PUBLISHER_INFO() {}


  bool MEDIA_PUBLISHER_INFO::loadFromJson(const std::string & json)
  {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["publisherName"].IsString() &&
        d["publisherURL"].IsString() &&
        d["favIconURL"].IsString() &&
        d["channelName"].IsString() &&
        d["publisher"].IsString() &&
        d["twitch_event"].IsString() &&
        d["twitch_time"].IsString() &&
        d["twitch_status"].IsString());
    }

    if (false == error)
    {
      publisherName_ = d["publisherName"].GetString();
      publisherURL_ = d["publisherURL"].GetString();
      favIconURL_ = d["favIconURL"].GetString();
      channelName_ = d["channelName"].GetString();
      publisher_ = d["publisher"].GetString();
      twitchEventInfo_.event_ = d["twitch_event"].GetString();
      twitchEventInfo_.time_ = d["twitch_time"].GetString();
      twitchEventInfo_.status_ = d["twitch_status"].GetString();
    }
    return !error;
  }

  void saveToJson(JsonWriter & writer, const MEDIA_PUBLISHER_INFO& data)
  {
    writer.StartObject();

    writer.String("publisherName");
    writer.String(data.publisherName_.c_str());

    writer.String("publisherURL");
    writer.String(data.publisherURL_.c_str());

    writer.String("favIconURL");
    writer.String(data.favIconURL_.c_str());

    writer.String("channelName");
    writer.String(data.channelName_.c_str());

    writer.String("publisher");
    writer.String(data.publisher_.c_str());

    writer.String("twitch_event");
    writer.String(data.twitchEventInfo_.event_.c_str());

    writer.String("twitch_time");
    writer.String(data.twitchEventInfo_.time_.c_str());

    writer.String("twitch_status");
    writer.String(data.twitchEventInfo_.status_.c_str());

    writer.EndObject();
  }

/////////////////////////////////////////////////////////////////////////////
  BATCH_PROOF::BATCH_PROOF() {}

  BATCH_PROOF::~BATCH_PROOF() {}

/////////////////////////////////////////////////////////////////////////////
  void split(std::vector<std::string>& tmp, std::string query, char delimiter)
  {
    std::stringstream ss(query);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
      if (query[0] != '\n') {
        tmp.push_back(item);
      }
    }
  }

  bool getJSONValue(const std::string& fieldName, const std::string& json, std::string & value) {

    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError() || ( false == d.HasMember(fieldName.c_str()) );
    if (false == error)
    {
      value = d[fieldName.c_str()].GetString();
    }
    return !error;
}

  bool getJSONList(const std::string& fieldName, const std::string& json, std::vector<std::string> & value) {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError() || (false == ( d.HasMember(fieldName.c_str())  &&  d[fieldName.c_str()].IsArray() ) );
    if (false == error)
    {
      for (auto & i : d[fieldName.c_str()].GetArray())
      {
        value.push_back(i.GetString());
      }
    }
    return !error;
  }

  bool getJSONTwitchProperties(const std::string& json, std::vector<std::map<std::string, std::string>>& parts) {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      for (auto & i : d.GetArray())
      {
        const char * event_field = "event";
        std::map<std::string, std::string> eventmap;

        auto obj = i.GetObject();
        if (obj.HasMember(event_field))
        {
          eventmap[event_field] = obj[event_field].GetString();
        }

        const char * props_field = "properties";
        if (obj.HasMember(props_field))
        {
          eventmap[props_field] = "";

          const char * channel_field = "channel";
          if (obj[props_field].HasMember(channel_field)) {
            eventmap[channel_field] = obj[props_field][channel_field].GetString();
          }

          const char * vod_field = "vod";
          if (obj[props_field].HasMember(vod_field)) {
            eventmap[vod_field] = obj[props_field][vod_field].GetString();
          }

          const char * time_field = "time";
          if (obj[props_field].HasMember(time_field)) {
            double d = obj[props_field][time_field].GetDouble();
            eventmap[time_field] = std::to_string(d);
          }
        }
        parts.push_back(eventmap);
      } //for (auto & i : d.GetArray())

    }
    return !error;
  }

  bool getJSONBatchSurveyors(const std::string& json, std::vector<std::string>& surveyors) {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      for (auto & i : d.GetArray())
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        i.Accept(writer);
        std::string surveyor = sb.GetString();
        surveyors.push_back(surveyor);
      }
    }
    return !error;
  }

  bool getJSONRates(const std::string& json, std::map<std::string, double>& rates) {
    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["rates"].IsObject() &&
        d["rates"].HasMember("ETH") &&
        d["rates"].HasMember("LTC") &&
        d["rates"].HasMember("BTC") &&
        d["rates"].HasMember("USD") &&
        d["rates"].HasMember("EUR"));
    }

    if (false == error)
    {
      for (auto & i : d["rates"].GetObject())
      {
        rates.insert(std::make_pair(i.name.GetString(), i.value.GetDouble()));
      }
    }
    return !error;
  }

  bool getJSONTransaction(const std::string& json, TRANSACTION_ST& transaction) {

    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["paymentStamp"].IsDouble() &&
        d["probi"].IsString() &&
        d["altcurrency"].IsString() );
    }

    if (false == error)
    {
      double stamp = d["paymentStamp"].GetDouble();
      transaction.submissionStamp_ = std::to_string((unsigned long long)stamp);
      transaction.contribution_probi_ = d["probi"].GetString();
      transaction.contribution_altcurrency_ = d["altcurrency"].GetString();
    }
    return !error;
  }

  bool getJSONUnsignedTx(const std::string& json, UNSIGNED_TX& unsignedTx) {
   rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["unsignedTx"].IsObject());
    }

    if (false == error)
    {
      unsignedTx.amount_ = d["unsignedTx"]["denomination"]["amount"].GetString();
      unsignedTx.currency_ = d["unsignedTx"]["denomination"]["currency"].GetString();
      unsignedTx.destination_ = d["unsignedTx"]["destination"].GetString();
    }
    return !error;
  }

  bool getJSONPublisherVerified(const std::string& json, bool& verified) {
    verified = false;

    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["properties"].IsObject());
    }

    if (false == error)
    {
      verified = d["properties"]["verified"].GetBool();
    }
    return !error;
  }

  bool getJSONWalletInfo(const std::string& json, WALLET_INFO_ST& walletInfo,
        std::string& fee_currency, double& fee_amount, unsigned int& days) {
   rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["wallet"].IsObject() &&
        d["payload"].IsObject() );
    }

    if (false == error)
    {
      walletInfo.paymentId_ = d["wallet"]["paymentId"].GetString();
      walletInfo.addressBAT_ = d["wallet"]["addresses"]["BAT"].GetString();
      walletInfo.addressCARD_ID_ = d["wallet"]["addresses"]["CARD_ID"].GetString();
      walletInfo.addressETH_ = d["wallet"]["addresses"]["ETH"].GetString();
      walletInfo.addressLTC_ = d["wallet"]["addresses"]["LTC"].GetString();

      days = d["payload"]["adFree"]["days"].GetUint();
      const auto & fee = d["payload"]["adFree"]["fee"].GetObject();
      auto itr = fee.MemberBegin();
      if (itr != fee.MemberEnd() )
      {
        fee_currency = itr->name.GetString();
        fee_amount = itr->value.GetDouble();
      }
    }
    return !error;
  }

  bool getJSONPublisherTimeStamp(const std::string& json, uint64_t& publisherTimestamp) {
    publisherTimestamp = 0;

    rapidjson::Document d;
    d.Parse(json.c_str());

    //has parser errors or wrong types
    bool error = d.HasParseError();
    if (false == error)
    {
      error = !(d["timestamp"].IsUint64());
    }

    if (false == error)
    {
      publisherTimestamp = d["timestamp"].GetUint64();
    }
    return !error;
  }

  std::vector<uint8_t> generateSeed() {
    //std::ostringstream seedStr;

    std::vector<uint8_t> vSeed(SEED_LENGTH);
    std::random_device r;
    std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
    auto rand = std::bind(std::uniform_int_distribution<>(0, UCHAR_MAX), std::mt19937(seed));

    std::generate_n(vSeed.begin(), SEED_LENGTH, rand);
    return vSeed;
  }

  std::vector<uint8_t> getHKDF(const std::vector<uint8_t>& seed) {
    DCHECK(!seed.empty());
    std::vector<uint8_t> out(SEED_LENGTH);
    int hkdfRes = HKDF(&out.front(), SEED_LENGTH, EVP_sha512(), &seed.front(), seed.size(),
      braveledger_ledger::g_hkdfSalt, SALT_LENGTH, nullptr, 0);

    DCHECK(hkdfRes);
    DCHECK(!seed.empty());
    return out;
  }

  void getPublicKeyFromSeed(const std::vector<uint8_t>& seed,
        std::vector<uint8_t>& publicKey, std::vector<uint8_t>& secretKey) {
    DCHECK(!seed.empty());
    publicKey.resize(crypto_sign_PUBLICKEYBYTES);
    secretKey = seed;
    secretKey.resize(crypto_sign_SECRETKEYBYTES);

    crypto_sign_keypair(&publicKey.front(), &secretKey.front(), 1);

    DCHECK(!publicKey.empty() && !secretKey.empty());
  }

  std::string uint8ToHex(const std::vector<uint8_t>& in) {
    std::ostringstream res;
    for (size_t i = 0; i < in.size(); i++) {
      res << std::setfill('0') << std::setw(sizeof(uint8_t) * 2)
         << std::hex << (int)in[i];
    }
    return res.str();
  }


  std::string stringify(std::string* keys, std::string* values, const unsigned int& size) {
    rapidjson::StringBuffer buffer;
    JsonWriter writer(buffer);
    writer.StartObject();

    for (unsigned int i = 0; i < size; i++)
    {
      writer.String(keys[i].c_str());
      writer.String(values[i].c_str());
    }

    writer.EndObject();
    return buffer.GetString();
  }

  std::string stringifyUnsignedTx(const UNSIGNED_TX& unsignedTx) {
    rapidjson::StringBuffer buffer;
    JsonWriter writer(buffer);
    writer.StartObject();

    writer.String("denomination");
    writer.StartObject();

    writer.String("amout");
    writer.String(unsignedTx.amount_.c_str());

    writer.String("currency");
    writer.String(unsignedTx.currency_.c_str());
    writer.EndObject();

    writer.String("destination");
    writer.String(unsignedTx.destination_.c_str());

    writer.EndObject();
    return buffer.GetString();
  }

  std::string stringifyRequestCredentialsSt(const REQUEST_CREDENTIALS_ST& request_credentials) {

    rapidjson::StringBuffer buffer;
    JsonWriter writer(buffer);
    writer.StartObject(); //root

    writer.String("requestType");
    writer.String(request_credentials.requestType_.c_str());

    writer.String("request");
    writer.StartObject(); //request

    writer.String("headers");
    writer.StartObject(); //headers

    writer.String("digest");
    writer.String(request_credentials.request_headers_digest_.c_str());

    writer.String("signature");
    writer.String(request_credentials.request_headers_signature_.c_str());

    writer.EndObject(); //headers

    writer.String("body");
    writer.StartObject(); //body

    writer.String("currency");
    writer.String(request_credentials.request_body_currency_.c_str());

    writer.String("label");
    writer.String(request_credentials.request_body_label_.c_str());

    writer.String("publicKey");
    writer.String(request_credentials.request_body_publicKey_.c_str());

    writer.EndObject(); //body

    writer.String("octets");
    writer.String(request_credentials.request_body_octets_.c_str());

    writer.EndObject(); //request

    writer.String("proof");
    writer.String(request_credentials.proof_.c_str());
    writer.EndObject(); //root
    return buffer.GetString();
  }

  std::string stringifyReconcilePayloadSt(const RECONCILE_PAYLOAD_ST& reconcile_payload) {
    rapidjson::StringBuffer buffer;
    JsonWriter writer(buffer);
    writer.StartObject(); //root

    writer.String("requestType");
    writer.String(reconcile_payload.requestType_.c_str());

    writer.String("signedTx");
    writer.StartObject(); //signedTx

    writer.String("headers");
    writer.StartObject(); //headers

    writer.String("digest");
    writer.String(reconcile_payload.request_signedtx_headers_digest_.c_str());

    writer.String("signature");
    writer.String(reconcile_payload.request_signedtx_headers_signature_.c_str());

    writer.EndObject(); //headers

    writer.String("body");
    writer.StartObject(); //body


    writer.String("denomination");
    writer.StartObject(); //denomination

    writer.String("amount");
    writer.String(reconcile_payload.request_signedtx_body_.amount_.c_str());

    writer.String("currency");
    writer.String(reconcile_payload.request_signedtx_body_.currency_.c_str());

    writer.EndObject(); //denomination

    writer.String("destination");
    writer.String(reconcile_payload.request_signedtx_body_.destination_.c_str());

    writer.EndObject(); //body

    writer.String("octets");
    writer.String(reconcile_payload.request_signedtx_octets_.c_str());

    writer.EndObject(); //signedTx

    writer.String("surveyorId");
    writer.String(reconcile_payload.request_surveyorId_.c_str());

    writer.String("viewingId");
    writer.String(reconcile_payload.request_viewingId_.c_str());

    writer.EndObject(); //root
    return buffer.GetString();
  }

  std::vector<uint8_t> getSHA256(const std::string& in) {
    std::vector<uint8_t> res(SHA256_DIGEST_LENGTH);
    SHA256((uint8_t*)in.c_str(), in.length(), &res.front());
    return res;
  }

  std::string getBase64(const std::vector<uint8_t>& in) {
    std::string res;
    size_t size = 0;
    if (!EVP_EncodedLength(&size, in.size())) {
      DCHECK(false);
      LOG(ERROR) << "EVP_EncodedLength failure in getBase64";

      return "";
    }
    std::vector<uint8_t> out(size);
    int numEncBytes = EVP_EncodeBlock(&out.front(), &in.front(), in.size());
    DCHECK(numEncBytes != 0);
    res = (char*)&out.front();
    return res;
  }

  bool getFromBase64(const std::string& in, std::vector<uint8_t> & out) {
    bool succeded = true;
    size_t size = 0;
    if (!EVP_DecodedLength(&size, in.length())) {
      DCHECK(false);
      LOG(ERROR) << "EVP_DecodedLength failure in getFromBase64";
      succeded = false;
    }

    if (succeded)
    {
      out.resize(size);
      int numDecBytes = EVP_DecodeBase64(&out.front(), &size, size, (const uint8_t*)in.c_str(), in.length());
      DCHECK(numDecBytes != 0);
      LOG(ERROR) << "!!!decoded size == " << size;

      if (0 == numDecBytes)
      {
        succeded = false;
        out.clear();
      }
    }
    return succeded;
  }

  std::string sign(std::string* keys, std::string* values, const unsigned int& size,
      const std::string& keyId, const std::vector<uint8_t>& secretKey) {
    std::string headers;
    std::string message;
    for (unsigned int i = 0; i < size; i++) {
      if (0 != i) {
        headers += " ";
        message += "\n";
      }
      headers += keys[i];
      message += keys[i] + ": " + values[i];
    }
    std::vector<uint8_t> signedMsg(crypto_sign_BYTES + message.length());

    unsigned long long signedMsgSize = 0;
    crypto_sign(&signedMsg.front(), &signedMsgSize, (const unsigned char*)message.c_str(), (unsigned long long)message.length(), &secretKey.front());

    std::vector<uint8_t> signature(crypto_sign_BYTES);
    std::copy(signedMsg.begin(), signedMsg.begin() + crypto_sign_BYTES, signature.begin());

    return "keyId=\"" + keyId + "\",algorithm=\"" + SIGNATURE_ALGORITHM +
      "\",headers=\"" + headers + "\",signature=\"" + getBase64(signature) + "\"";
  }

  uint64_t currentTime() {
    return time(0);
  }

  void saveState(const CLIENT_STATE_ST& state) {

    std::string data;
    braveledger_bat_helper::saveToJsonString(state, data);
    //LOG(ERROR) << "!!!saveState == " << data;

    std::string ledger_state_file_path;
    std::string root;
    braveledger_bat_helper::getHomeDir(root);
    braveledger_bat_helper::appendPath(root, LEDGER_STATE_FILENAME, ledger_state_file_path);

    auto runnable = braveledger_bat_helper::bat_fun_binder(&braveledger_bat_helper::writeFileNoReturn, ledger_state_file_path, data);
    braveledger_bat_helper::PostTask(runnable);
  }

  void loadState(ReadStateCallback callback) {

    auto runnable = braveledger_bat_helper::bat_fun_binder(&braveledger_bat_helper::readStateFile, callback);
    braveledger_bat_helper::PostTask(runnable);
  }

  void savePublisherState(const PUBLISHER_STATE_ST& state) {
    std::string data;
    braveledger_bat_helper::saveToJsonString(state, data);

    std::string ledger_pub_state_file_path;
    std::string root;
    braveledger_bat_helper::getHomeDir(root);
    braveledger_bat_helper::appendPath(root, LEDGER_PUBLISHER_STATE_FILENAME, ledger_pub_state_file_path);

    auto runnable = braveledger_bat_helper::bat_fun_binder(&braveledger_bat_helper::writeFileNoReturn, ledger_pub_state_file_path, data);
    braveledger_bat_helper::PostTask(runnable);
  }

  void loadPublisherState(ReadPublisherStateCallback callback) {

    auto runnable = braveledger_bat_helper::bat_fun_binder(&braveledger_bat_helper::readPublisherStateFile, callback);
    braveledger_bat_helper::PostTask(runnable);
  }

  void getUrlQueryParts(const std::string& query, std::map<std::string, std::string>& parts) {
    std::vector<std::string> vars;
    split(vars, query, '&');
    for (size_t i = 0; i < vars.size(); i++) {
      std::vector<std::string> var;
      split(var, vars[i], '=');
      if (var.size() != 2) {
        continue;
      }
      std::string varName;
      std::string varValue;
      DecodeURLChars(var[0], varName);
      DecodeURLChars(var[1], varValue);
      parts[varName] = varValue;
    }
  }

  void getTwitchParts(const std::string& query, std::vector<std::map<std::string, std::string>>& parts) {
    size_t pos = query.find("data=");
    if (std::string::npos != pos && query.length() > 5) {
      std::string varValue;
      DecodeURLChars(query.substr(5), varValue);
      std::vector<uint8_t> decoded;
      bool succeded = braveledger_bat_helper::getFromBase64(varValue, decoded);
      if (succeded)
      {
        decoded.push_back((uint8_t)'\0');
        braveledger_bat_helper::getJSONTwitchProperties((char*)&decoded.front(), parts);
      }
      else
      {
        LOG(ERROR) << "getTwitchParts failed in getFromBase64";
      }
    }
  }

  std::string getMediaId(const std::map<std::string, std::string>& data, const std::string& type) {
    if (YOUTUBE_MEDIA_TYPE == type) {
      std::map<std::string, std::string>::const_iterator iter = data.find("docid");
      if (iter != data.end()) {
        return iter->second;
      }
    } else if (TWITCH_MEDIA_TYPE == type) {
      std::map<std::string, std::string>::const_iterator iter = data.find("event");
      if (iter != data.end() && data.find("properties") != data.end()) {
        for (size_t i = 0; i < braveledger_ledger::_twitch_events_array_size; i++) {
          if (iter->second == braveledger_ledger::_twitch_events[i]) {
            iter = data.find("channel");
            std::string id("");
            if (iter != data.end()) {
              id = iter->second;
            }
            iter = data.find("vod");
            if (iter != data.end()) {
              std::string idAddition(iter->second);
              std::remove(idAddition.begin(), idAddition.end(), 'v');
              id += "_vod_" + idAddition;
            }

            return id;
          }
        }
      }
    }

    return "";
  }

  std::string getMediaKey(const std::string& mediaId, const std::string& type) {
    return type + "_" + mediaId;
  }

  uint64_t getMediaDuration(const std::map<std::string, std::string>& data, const std::string& mediaKey, const std::string& type) {
    uint64_t duration = 0;

    if (YOUTUBE_MEDIA_TYPE == type) {
      std::map<std::string, std::string>::const_iterator iterSt = data.find("st");
      std::map<std::string, std::string>::const_iterator iterEt = data.find("et");
      if (iterSt != data.end() && iterEt != data.end()) {
        std::vector<std::string> startTime;
        std::vector<std::string> endTime;
        split(startTime, iterSt->second, ',');
        split(endTime, iterEt->second, ',');
        if (startTime.size() != endTime.size()) {
          return 0;
        }
        double tempTime = 0;
        for (size_t i = 0; i < startTime.size(); i++) {
          std::stringstream tempET(endTime[i]);
          std::stringstream tempST(startTime[i]);
          double st = 0;
          double et = 0;
          tempET >> et;
          tempST >> st;
          tempTime = et - st;
        }
        duration = (uint64_t)(tempTime * 1000.0);
      }
    } else if (TWITCH_MEDIA_TYPE == type) {
      // We set the correct duration for twitch in BatGetMedia class
      duration = 0;
    }

    return duration;
  }

  void readStateFile(ReadStateCallback callback) {
    std::string file_path;
    std::string root;
    braveledger_bat_helper::getHomeDir(root);
    std::ostringstream data;

    braveledger_bat_helper::appendPath(root, LEDGER_STATE_FILENAME, file_path);
    bool succeded = braveledger_bat_helper::readFile(file_path, data);
    if (succeded)
    {
      CLIENT_STATE_ST state;
      braveledger_bat_helper::loadFromJson(state, data.str());
      braveledger_bat_helper::run_runnable (callback, true, std::cref(state));
    }
    else
    {
      CLIENT_STATE_ST temp;
      braveledger_bat_helper::run_runnable(callback, false, std::cref(temp));
    }
  }


  void readPublisherStateFile(ReadPublisherStateCallback callback) {
    std::string file_path;
    std::string root;
    braveledger_bat_helper::getHomeDir(root);
    std::ostringstream data;

    braveledger_bat_helper::appendPath(root, LEDGER_PUBLISHER_STATE_FILENAME, file_path);
    bool succeded = braveledger_bat_helper::readFile(file_path, data);
    if (succeded)
    {
      PUBLISHER_STATE_ST state;
      braveledger_bat_helper::loadFromJson(state, data.str());
      braveledger_bat_helper::run_runnable(callback, true, std::cref(state));
    }
    else
    {
      PUBLISHER_STATE_ST temp;
      braveledger_bat_helper::run_runnable(callback, false, std::cref(temp));
    }
  }

  void writeFileNoReturn(const std::string & path, const std::string& data)
  {
    //ignoring return
    writeFile(path, data);
  }
}  // namespace braveledger_bat_helper
