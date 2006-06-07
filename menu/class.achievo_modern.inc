<?php

atkimport("atk.menu.atkplainmenu");

class achievo_modern extends atkPlainMenu 
{
  function render()
  {
    $theme = &atkinstance("atk.ui.atktheme");
    if ($theme->getAttribute('modern_menu_compat'))
    {
      global $g_menu;
      $atkmenutop="main";

      $menuitems = $this->getMenuItems($g_menu,$atkmenutop);

      $page = &atknew("atk.ui.atkpage");
      $page->register_style($theme->stylePath("style.css"));
      $page->register_style($theme->stylePath("menu.css"));
      $page->register_script(atkconfig("atkroot")."atk/javascript/menuload.js");

      $ui = &atkinstance("atk.ui.atkui");

      $box = $ui->renderBox(array("title"=>$this->getMenuTranslation($atkmenutop),
      "menuitems"=>$menuitems,
      'menutop'=>$atkmenutop,
      'g_menu'=>$g_menu,
      'atkmenutop'=>($_REQUEST['atkmenutop']?$_REQUEST['atkmenutop']:$atkmenutop)),"menu");

      $page->addContent($box);

      return $page->render("Menu", true);
    }
    else 
    {
      $oldmenu = &atknew('menu.achievo');
      return $oldmenu->render();
    }
  }
  
  function getMenuItems($menu, $menutop)
  {
    if (is_array($menu[$menutop]))
    {
      usort($menu[$menutop],array("atkplainmenu","menu_cmp"));
      $menuitems = array();
      foreach ($menu[$menutop] as $menuitem)
      {
        $this->setEnabled($menuitem);

        $this->addSubMenuForMenuitem($menu,$menuitem);
        if ($menuitem['name']!=='-')
        {
          $menuitem['name'] = $this->getMenuTranslation($menuitem['name'],$menuitem['module']);
        }
        $menuitems[] = $menuitem;
      }
    }
    return $menuitems;
  }
  
  function addSubMenuForMenuItem($menu,&$menuitem)
  {
    // submenu
    if ($menu[$menuitem['name']])
    {
      $enablesubmenu=false;
      for ($i=0;$i<count($menu[$menuitem['name']]);$i++)
      {
        if ($this->setEnabled($menu[$menuitem['name']][$i]))
        {
          $enablesubmenu = true;
        }
      }
      if ($enablesubmenu)
      {
        $menuitem['submenu'] = $menu[$menuitem['name']];
        foreach ($menuitem['submenu'] as $submenukey=>$submenuitem)
        {
          if ($menuitem['submenu'][$submenukey]['name']!=='-')
          {
            $menuitem['submenu'][$submenukey]['name'] = $this->getMenuTranslation($submenuitem['name'],$submenuitem['module']);
          }
        }
      }
    }
  }
  
  function setEnabled(&$menuitem)
  {
    if (is_array($menuitem["enable"]))
    {
      $enabled = false;
      for ($j=0;$j<(count($menuitem["enable"])/2);$j++)
      {
        $enabled |= is_allowed($menuitem["enable"][(2*$j)],$menuitem["enable"][(2*$j)+1]);
      }
      $menuitem["enable"]=$enabled;
    }
    else 
      $enabled = $menuitem["enable"];
      
    return ($menuitem["name"]==='-'?false:$enabled);
  }

  function getHeader($atkmenutop)
  {
    $prj='';
    if ($atkmenutop == "projectmanagement")
    {
      global $g_sessionManager;
      include_once "achievotools.inc";
      $projects = $g_sessionManager->getValue("selectedprj","globals");

      if (count($projects) == 0)
      {
        //updateSelectedProjects();
        $projects = $g_sessionManager->getValue("selectedprj","globals");
      }

      $prj  = atktext("project_select").":";
      $prj .="<FORM><SELECT style=\"width: 150px\" name=\"selectedproject\" onchange=\"reloadProjects(this);\">";
      $prj .= "<OPTION value=\"0\">".atktext("project_select_none")."</OPTION>";

      $selectedproject = array_key_exists("selectedproject", $_REQUEST) ? $_REQUEST["selectedproject"] : null;
      for ($i=0;$i < count($projects); $i++)
      {
        $prj .= "<OPTION value=\"".$projects[$i]['projectid']."\"";
        if ($selectedproject == $projects[$i]['projectid']) $prj .=" selected";
        $prj .= ">".$projects[$i]['projectname']."</OPTION>";
      }
      $prj .="</SELECT></FORM>";
    }
    return $prj;
  }
}

?>