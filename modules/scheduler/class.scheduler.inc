<?php

/**
 * Module scheduler
 * Class schedule
 *
 * This class makes it possible to schedule a appointment.
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 * TODO: $scheduler->isAvailable("sandy", "2003-10-28", "11.00", "13.00",30);
 *  heeft sandy op 28 oktober tussen 11 en 13 minimaal een half uur vrij in zijn agenda
 *
 */



  useattrib("atkdateattribute");
  useattrib("atktextattribute");
  useattrib("atkboolattribute");
  useattrib("atktimeattribute");
  useattrib("atklistattribute");
  useattrib("atkdummyattribute");
  useattrib("atkflagattribute");
  useattrib("atkdurationattribute");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  useattrib("atkfuzzysearchattribute");
  //useattrib("scheduler.schedulerradioattribute");
  //useattrib("scheduler.schedulerlistattribute");
  //useattrib("scheduler.schedulerboolattribute");

  class scheduler extends atkNode
  {
    var $m_viewdate;
    var $m_small_viewdate;
    var $m_viewUser;
    var $m_view;
    var $m_employees;

    // Default minute step
    var $m_minute_step = 5;


    function scheduler()
    {
      global $g_sessionManager,$ATK_VARS;
      $this->m_view = $g_sessionManager->pageVar("view");
      $this->m_viewUser = $g_sessionManager->pageVar("user");
      $this->m_employees = $g_sessionManager->pageVar("employee");
      $this->m_itemDate = $g_sessionManager->stackVar("itemdate");




      $user = getUser();
      $userprefs = $this->getUserSchedulerPrefs($user["id"]);
      atk_var_dump($userprefs);

      $eventtime = $userprefs["default_eventtime"];
      $user_eventtime = ($eventtime["hours"]*60)+$eventtime["minutes"];

      $time = $g_sessionManager->stackVar("time");
      for($i=0;$i<60;$i+=$this->m_minute_step) $m_arr_minute_step[] = sprintf("%02d",$i);
      if ($time!="")
      {
        $default_start_time = $time; // passed from the url.

        // endtime is half an hour later than starttime by default
        $default_end_time = strftime("%H:%M", mktime(substr($time,0,2), substr($time,3,2)+$user_eventtime));
      }
      else
      {
        $default_start_time = strftime("%H:%M");
        $default_end_time = strftime("%H:%M", mktime(substr($default_start_time,0,2),substr($default_end_time,3,2)+$user_eventtime));
      }
      $day_arr = array();
      for($i=1;$i<32;$i++) $day_arr[]=$i;

      $month_arr_value=array();
      for($i=1;$i<13;$i++) $month_arr_value[]=$i;
      $month_arr_disp = array();
      $month_arr_disp[] = text("januari");
      $month_arr_disp[] = text("februari");
      $month_arr_disp[] = text("march");
      $month_arr_disp[] = text("april");
      $month_arr_disp[] = text("may");
      $month_arr_disp[] = text("june");
      $month_arr_disp[] = text("july");
      $month_arr_disp[] = text("august");
      $month_arr_disp[] = text("september");
      $month_arr_disp[] = text("october");
      $month_arr_disp[] = text("november");
      $month_arr_disp[] = text("december");



      $this->atkNode("scheduler", NF_ADD_LINK);

      $this->add(new atkAttribute("id"              , AF_AUTOKEY));
      $this->add(new atkOneToManyRelation("attendees", "scheduler.scheduler_attendees", "schedule_id", AF_HIDE_LIST|AF_CASCADE_DELETE));
      $this->add(new atkDummyAttribute("hint", text("attendee_fuzzyhint"), AF_HIDE_LIST|AF_HIDE_VIEW));
      $this->add(new atkfuzzySearchAttribute("attendee_empsearch", "employee.employee", "storeAttendees", FSA_MULTISELECT));
      $this->add(new atkfuzzySearchAttribute("attendee_contactsearch", "organization.contact", "storeAttendees", FSA_MULTISELECT));

      $this->add(new atkAttribute("title"          , AF_SEARCHABLE|AF_OBLIGATORY, 50));
      $this->add(new atkAttribute("location"        , AF_SEARCHABLE, 50));
      $this->add(new atkTextAttribute("description"));
      $tmp_recur = &new atkRadioAttribute("recur",array("once","daily","weekly","monthly_date","monthly_day","yearly_date","yearly_day"),array());
      $tmp_recur->addOnChangeHandler("change_recur(newvalue);");
      $this->add($tmp_recur,"cyclus");
      $this->add(new atkDateAttribute("startdate"  , "F d Y","d F Y", 0, 0));
      $this->add(new atkDateAttribute("enddate"    , "F d Y","d F Y", 0, 0));
      $tmp_allday = &new atkBoolAttribute("allday");
      $tmp_allday->addOnChangeHandler("change_time();");
      $this->add($tmp_allday);
      $this->add(new atkTimeAttribute("starttime"  , 0, 23, $m_arr_minute_step, $default_start_time, 0));
      $this->add(new atkTimeAttribute("endtime"    ,0, 23, $m_arr_minute_step, $default_end_time, 0));



      $this->add(new atkDateAttribute("cyclus_startdate"  , "F d Y","d F Y", 0, 0),"cyclus");
      $this->add(new atkDateAttribute("cyclus_enddate"    , "F d Y","d F Y", 0, 0),"cyclus");
      $this->add(new atkListAttribute("startday",$day_arr,$day_arr,AF_OBLIGATORY|AF_NO_TRANSLATION),"cyclus");
      $this->add(new atkListAttribute("endday",$day_arr,$day_arr,AF_OBLIGATORY|AF_NO_TRANSLATION),"cyclus");
      $this->add(new atkListAttribute("startmonth",$month_arr_disp,$month_arr_value,AF_OBLIGATORY),"cyclus");
      $this->add(new atkListAttribute("endmonth",$month_arr_disp,$month_arr_value,AF_OBLIGATORY),"cyclus");

      $this->add(new atkListAttribute("every",array(1,2,3,4,5,6,7,8,9,10),array(1,2,3,4,5,6,7,8,9,10),AF_OBLIGATORY|AF_NO_TRANSLATION),"cyclus");

      $this->add(new atkListAttribute("month_time",array("first","second","third","fourth"),array(1,2,3,4),AF_OBLIGATORY),"cyclus");
      $this->add(new atkFlagAttribute("weekday",array("sunday","monday","thuesday","wednedsday","thursday","friday","saturday"),array(1,2,4,8,16,32,64)),"cyclus");
      $this->add(new atkListAttribute("reminder",array("1 day","2 day", "3 days"),array(1,2,3)));


      //$this->add(new atkManyToOneRelation("schedulecategory", "scheduler.scheduler_category",AF_OBLIGATORY|AF_RELATION_AUTOLINK));
      $this->add(new atkBoolAttribute("private"));
      $this->add(new atkBoolAttribute("all_users"));
      //$this->add(new atkListAttribute("status", array("confirmed", "unconfirmed", "cancelled"), ""));

      $this->add(new atkOneToManyRelation("notes", "scheduler.scheduler_notes", "schedule_id", AF_HIDE_LIST|AF_HIDE_ADD|AF_CASCADE_DELETE));
      $this->add(new atkManyToOneRelation("owner", "employee.employee", AF_READONLY|AF_OBLIGATORY));
      $this->add(new atkBoolAttribute("owner_attendee"));

      atkdebug("VIEW: ".$this->m_view." - ".$this->m_viewUser);
      if($this->m_viewUser=="" && $this->m_view!="employeeview")
      {
        if ($userprefs["default_view"]!="*")
        {
          $this->m_viewUser = $user["id"];
        }
        else
        {
         // User has opted to show all users by default.
          $this->m_viewUser = "all";
        }
      }

      if (trim($this->m_view) == "")
      {
         $this->m_view = $userprefs["default_view"];
      }
      atkdebug("VIEW: ".$this->m_view." - ".$this->m_viewUser);



      $this->m_viewdate = $g_sessionManager->stackVar("viewdate");
      $this->m_small_viewdate = $g_sessionManager->stackVar("small_viewdate");

      if(is_array($this->m_viewdate))
      {
        $this->m_viewdate = sprintf('%04d-%02d-%02d',$this->m_viewdate['year'],$this->m_viewdate['month'],$this->m_viewdate['day']);
      }
      elseif ($this->m_viewdate=="")
      {
        $this->m_viewdate = strftime("%Y-%m-%d");
      }

      $this->addAllowedAction("view");

      $this->setTable("scheduler_scheduler", "scheduler_scheduler");
      if ($this->m_view == "day")
      {
        $this->setOrder("startdate DESC");
      }
      else
      {
        $this->setOrder("startdate DESC, starttime");
      }
    }

    // Display functions
    function starttime_display($rec)
    {
      return $rec["starttime"]["hours"].":".$rec["starttime"]["minutes"];
    }

    function endtime_display($rec)
    {
      return $rec["endtime"]["hours"].":".$rec["endtime"]["minutes"];
    }


    function initial_values()
    {
      $user = getUser();
      $initial["owner"]["id"] = $user["id"];
      $initial["private"] = 0;
      $initial["status"] = "unconfirmed";
      $initial["recur"]="once";
      $initial["startday"]=1;
      $initial["endday"]=1;
      $initial["startmonth"]=1;
      $initial["endmonth"]=1;
      $initial["every"]=1;
      $initial["month_time"]=1;
      $initial["weekday"]=0;

      if ($this->m_viewdate!="")
      {
        $arr_date = array("year"=>substr($this->m_viewdate, 0, 4),
                          "month"=>substr($this->m_viewdate, 5, 2),
                          "day"=>substr($this->m_viewdate, 8, 2));
        $initial["startdate"] = $arr_date;
        $initial["enddate"] = $arr_date;
        $initial["cyclus_startdate"]=$arr_date;
      }
      return $initial;
    }

    function allowed($action,$record="")
    {
      if($action=="edit" && $record["owner"]!=$this->m_user)
      {
        return false;
      }
      else
      {
        return parent::allowed($action,$record);
      }
    }

    function descriptor_def()
    {
      return "[title]";
    }


    // Validate functions
    function enddate_validate(&$record, $mode)
    {
      if ($record["enddate"]["year"] < $record["startdate"]["year"])
      {
        triggerError($record, "enddate", "error_date");
      }
      elseif ($record["enddate"]["year"] == $record["startdate"]["year"])
      {
        if ($record["enddate"]["month"] < $record["startdate"]["month"])
        {
          triggerError($record, "enddate", "error_date");
        }
        elseif ($record["enddate"]["month"] == $record["startdate"]["month"])
        {
          if ($record["enddate"]["day"] < $record["startdate"]["day"])
          {
            triggerError($record, "enddate", "error_date");
          }
        }
      }
    }

    function endtime_validate(&$record, $mode)
    {
      // check if start and enddate are the same else don't validate
      $startdate = $record["startdate"]["year"].$record["startdate"]["month"].$record["startdate"]["day"];
      $enddate = $record["enddate"]["year"].$record["enddate"]["month"].$record["enddate"]["day"];

      if($startdate==$enddate && $record["allday"]==0)
      {
        if ($record["endtime"]["hours"] < $record["starttime"]["hours"])
        {
          triggerError($record, "endtime", "error_time");
        }
        elseif ($record["endtime"]["hours"] == $record["starttime"]["hours"])
        {
          if ($record["endtime"]["minutes"] <= $record["starttime"]["minutes"])
          {
            triggerError($record, "endtime", "error_time");
          }
        }
      }
    }

    function every_edit($record="", $fieldprefix="",$mode)
    {
      $attrib = $this->getAttribute("every");
      $ret = $attrib->edit($record,$fieldprefix,$mode);
      $ret.= '&nbsp;<span id="every_day">day(s)</span><span id="every_week">week(s)</span><span id="every_month">month(s)</span><span id="every_year">year(s)</span>';

      return $ret;
    }

    // Page functions
    function adminPage($handler,$actions="")
    {
      $ui = &$this->getUi();
      if (is_object($ui))
      {
        $theme = &atkTheme::getInstance();
        $page = &$this->getPage();
        $page->register_style($theme->stylePath("style.css"));
        $page->register_style(moduledir("scheduler")."styles/scheduler.css");
        $page->head("Schedule");
        $page->body();

        if (method_exists($this, $this->m_view."view"))
        {
          $funcname = $this->m_view."view";
          $result = $this->$funcname();
        }
        else
        {
          $result = "Niet bestaande view (".$this->m_view." view)";
        }

        $page->addContent($ui->renderBox(array("title"=>text($this->m_type)." - ".text($this->m_view),
                                               "content"=>$result)));
      }
      else
      {
         atkerror("ui object failure");
      }
    }

    function editPage(&$handler,$record,$locked)
    {
      $page = &$this->getPage();
      $page->register_script(moduleDir("scheduler")."javascript/scheduler.js");
      $page->register_loadscript("change_recur();");
      return $handler->editPage($record,$locked);
    }

    function addPage(&$handler,$record=NULL)
    {
      $page = &$this->getPage();
      $page->register_script(moduleDir("scheduler")."javascript/scheduler.js");
      $page->register_loadscript("change_recur();");
      return $handler->addPage($record);
    }

    function action_delete(&$handler)
    {
      if (!empty($this->m_postvars['confirmsingle'])) $this->_doDeleteSingle();
      else $handler->action_delete();
    }


    function confirmDelete($atkselector, $locked)
    {
      $ui = &$this->getUi();

      $this->addStyle("style.css");

      if (is_array($atkselector))
        $atkselector_str = '(('. implode($atkselector, ') OR (').'))';
      else $atkselector_str = $atkselector;

      $formstart ='<form action="'.$_SERVER["PHP_SELF"].'?"'.SID.' method="post">';
      $formstart.=session_form();
      $formstart.='<input type="hidden" name="atkaction" value="delete">';
      $formstart.='<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">';
      $formstart.='<input type="hidden" name="atkselector" value="'.$atkselector_str.'">';
      $formstart.='<input type="hidden" name="itemdate" value="'.$this->m_postvars["itemdate"].'">';

      $buttons = array();

      $content = "";
      $recs = $this->selectDb($atkselector_str, "", "", "", array_merge($this->descriptorFields(),array("recur")));
      $record = $recs[0];

      if($record["recur"]!="once")
      {
        $buttons[] = '<input name="confirmsingle" class="btn" type="submit" value="'.atktext('delete_single').'">';
        $buttons[] = '<input name="confirm" class="btn" type="submit" value="'.atktext("delete_cyclus").'">';
      }
      else
      {
        $buttons[] = '<input name="confirm" class="btn" type="submit" value="'.atktext('yes').'">';

      }
      $buttons[] = '<input name="cancel" type="submit" class="btn_cancel" value="'.atktext('no').'">';

      $content.= '<br>'.$this->confirmActionText($atkselector, "delete", false);
      $action="delete";

      $output=$ui->renderAction($action, array("content"=>$content,
                                                "formstart"=>$formstart,
                                                "formend"=>'</form>',
                                                "buttons"=>$buttons));
      return $ui->renderBox(array("title"=>$this->actionTitle($action, $record),
                                  "content"=>$output));

    }

    function _doDeleteSingle()
    {
      $db = &$this->m_node->getDb();
      $rec = $this->m_node->selectDb($this->m_postvars['atkselector']);
      if($rec)
      {
        $name = "atk".atkconfig("database")."query";
        $query = atknew($name);
        $query->addTable('scheduler_cyclus_not');
        $query->addField('scheduler_id',$rec["scheduler_id"],'scheduler_cyclus_not');
        $query->addField('date',$this->m_postvars["atkitemdate"],'scheduler_cyclus_not');
        $sql = $query->buildInsert();
        $db->query($sql);
        $this->clearCache();
        $location = $this->m_node->feedbackUrl("delete", ACTION_SUCCESS);
        $this->m_node->redirect($location);
      }
      // Something is wrong here, the selectDb failed
      else
      {
        $db->rollback();
        $location = $this->m_node->feedbackUrl("delete",ACTION_FAILED,$db->getErrorMsg());
        $this->m_node->redirect($location);
      }
    }

    function dayview()
    {
      $dayview = &atknew("modules.scheduler.views.scheduler_dayview");
      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);
      $dayview->setViewDate($day,$month,$year);
      $day   = substr($this->m_small_viewdate, 8,2);
      $month = substr($this->m_small_viewdate, 5,2);
      $year  = substr($this->m_small_viewdate, 0,4);
      $dayview->setSmallViewDate($day,$month,$year);
      $dayview->setViewUser($this->m_viewUser);
      return $dayview->renderView();
    }

    function weekview()
    {
      $weekview = &atknew("modules.scheduler.views.scheduler_weekview");
      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);
      $weekview->setViewDate($day,$month,$year);
      $day   = substr($this->m_small_viewdate, 8,2);
      $month = substr($this->m_small_viewdate, 5,2);
      $year  = substr($this->m_small_viewdate, 0,4);
      $weekview->setSmallViewDate($day,$month,$year);
      $weekview->setViewUser($this->m_viewUser);
      return $weekview->renderView();

    }

    function monthview()
    {
      // viewdate is something like 2001-07-23
      $monthview = &atknew("modules.scheduler.views.scheduler_monthview");

      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);

      $monthview->setViewDate($day,$month,$year);
      $day   = substr($this->m_small_viewdate, 8,2);
      $month = substr($this->m_small_viewdate, 5,2);
      $year  = substr($this->m_small_viewdate, 0,4);
      $monthview->setSmallViewDate($day,$month,$year);

      $monthview->setViewUser($this->m_viewUser);
      return $monthview->renderView();
    }

    function yearview()
    {
      // viewdate is something like 2001-07-23
      $yearview = &atknew("modules.scheduler.views.scheduler_yearview");
      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);
      $yearview->setViewDate($day,$month,$year);
      $yearview->setViewUser($this->m_viewUser);
      return $yearview->renderView();
    }

    function employeeview()
    {
      $employeeview = &atknew("modules.scheduler.views.scheduler_employeeview");
      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);

      $employeeview->setViewDate($day,$month,$year);
      $employeeview->setEmployees($this->m_employees);
      return $employeeview->renderView();
    }


    // Record functions
    function postAdd($rec, $mode)
    {
      // insert the owner as first attendee.
      $db = &atkGetDb();
      if($rec["owner_attendee"]==1)
        $db->query("INSERT INTO scheduler_attendees (schedule_id, person_id) VALUES (".$rec["id"].", ".$rec["owner"]["id"].")");
      return true;
    }

    function storeCC($rec, $attendees)
    {
      $db = &atkGetDb();
      for ($i=0, $_i=count($attendees); $i<$_i; $i++)
      {
        $db->query("INSERT INTO achilles_bug_cc(bug_id, user) VALUES ('".$rec["id"]."', '".$attendees[$i]["id"]."')");
      }
    }

    function preAdd(&$rec)
    {
      $this->UpdateScheduleRec($rec);
    }

    function preUpdate(&$rec)
    {
      $this->UpdateScheduleRec($rec);
    }

    function UpdateScheduleRec(&$rec)
    {
      atkdebug("schedule::UpdateScheduleRec");
      atk_var_dump($rec);
      if($rec["recur"]=="once")
      {
        $rec["cyclus_startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus_enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["startday"]=1;
        $rec["endday"]=1;
        $rec["startmonth"]=1;
        $rec["endmonth"]=1;
        $rec["every"]=1;
        $rec["month_time"]=1;
        $rec["weekday"]=0;
      }
      elseif($rec["recur"]=="daily")
      {
        $rec["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["startday"]=1;
        $rec["endday"]=1;
        $rec["startmonth"]=1;
        $rec["endmonth"]=1;
        $rec["month_time"]=1;
        $rec["weekday"]=0;
      }
      elseif($rec["recur"]=="weekly")
      {
        $rec["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["startday"]=1;
        $rec["endday"]=1;
        $rec["startmonth"]=1;
        $rec["endmonth"]=1;
        $rec["month_time"]=1;
      }
      elseif($rec["recur"]=="monthly_date")
      {
        $rec["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["startmonth"]=1;
        $rec["endmonth"]=1;
        $rec["month_time"]=1;
        $rec["weekday"]=0;
      }
      elseif($rec["recur"]=="monthly_day")
      {
        $rec["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["startday"]=1;
        $rec["endday"]=1;
        $rec["startmonth"]=1;
        $rec["endmonth"]=1;
      }
      elseif($rec["recur"]=="yearly_date")
      {
        $rec["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["month_time"]=1;
        $rec["weekday"]=0;
      }
      elseif($rec["recur"]=="yearly_day")
      {
        $rec["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["startday"]=1;
        $rec["endday"]=1;
        //$rec["startmonth"]=1;
        $rec["endmonth"]=1;
      }
    }

    function getUserSchedulerPrefs($userid)
    {
       $userprefsnode = &atkNew("module.scheduler.scheduler_userpreferences");
       $userprefs = $userprefsnode->selectdb("userid='".$userid."'");
       return $userprefs[0];
    }
  }

?>