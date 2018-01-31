$( document ).ready(function() {

  $("div.headertitle").addClass("page-header");
  $("div.title").addClass("h1");

  /* move labels and groups out of header */
  $("div.headertitle").append($("div.title").children());

  // $('li > a[href="index.html"] > span').before("<i class='fa fa-cog'></i> ");
  // $('li > a[href="modules.html"] > span').before("<i class='fa fa-square'></i> ");
  // $('li > a[href="namespaces.html"] > span').before("<i class='fa fa-bars'></i> ");
  // $('li > a[href="annotated.html"] > span').before("<i class='fa fa-list-ul'></i> ");
  // $('li > a[href="classes.html"] > span').before("<i class='fa fa-book'></i> ");
  // $('li > a[href="inherits.html"] > span').before("<i class='fa fa-sitemap'></i> ");
  // $('li > a[href="functions.html"] > span').before("<i class='fa fa-list'></i> ");
  // $('li > a[href="functions_func.html"] > span').before("<i class='fa fa-list'></i> ");
  // $('li > a[href="functions_vars.html"] > span').before("<i class='fa fa-list'></i> ");
  // $('li > a[href="functions_enum.html"] > span').before("<i class='fa fa-list'></i> ");
  // $('li > a[href="functions_eval.html"] > span').before("<i class='fa fa-list'></i> ");
  $('img[src="ftv2ns.png"]').replaceWith('<span class="label label-danger">N</span> ');
  $('img[src="ftv2cl.png"]').replaceWith('<span class="label label-danger">C</span> ');

  $("ul.tablist").addClass("nav nav-pills nav-justified");
  $("ul.tablist").css("margin-top", "0.5em");
  $("ul.tablist").css("margin-bottom", "0.5em");
  $("li.current").addClass("active");
  $("iframe").attr("scrolling", "yes");

  $("#nav-path > ul").addClass("breadcrumb");

  $("table.params").addClass("table");
  $("div.ingroups").wrapInner("<span class='text-nowrap'></span>");
  $("div.levels").css("margin", "0.5em");
  $("div.levels > span").addClass("btn btn-default btn-xs");
  $("div.levels > span").css("margin-right", "0.25em");

  $("table.directory").addClass("table table-striped");
  $("div.summary > a").addClass("btn btn-default btn-xs");
  $("table.fieldtable").addClass("table");
  $(".fragment").addClass("well");
  $(".memitem").addClass("panel panel-default");
  $(".memproto").addClass("panel-heading");
  $(".memdoc").addClass("panel-body");
  $("span.mlabel").addClass("label label-info");

  $("table.memberdecls").addClass("table");
  $("[class^=memitem]").addClass("active");

  $("div.ah").addClass("btn btn-default");
  $("span.mlabels").addClass("pull-right");
  $("table.mlabels").css("width", "100%")
  // $("td.mlabels-right").addClass("pull-right");

  $("div.ttc").addClass("panel panel-primary");
  $("div.ttname").addClass("panel-heading");
  $("div.ttname a").css("color", 'white');
  $("div.ttdef,div.ttdoc,div.ttdeci").addClass("panel-body");

  $('div.fragment.well div.line:first').css('margin-top', '2px');
  $('div.fragment.well div.line:last').css('margin-bottom', '2px');

  $('table.doxtable').removeClass('doxtable').addClass('table table-striped table-bordered').each(function(){
    $(this).prepend('<thead></thead>');
    $(this).find('tbody > tr:first').prependTo($(this).find('thead'));

    $(this).find('td > span.success').parent().addClass('success');
    $(this).find('td > span.warning').parent().addClass('warning');
    $(this).find('td > span.danger').parent().addClass('danger');
  });

  if($('div.fragment.well div.ttc').length > 0)
  {
      $('div.fragment.well div.line:first').parent().removeClass('fragment well');
  }

  /* smart menus */
  var nav_container = $('#main-nav').detach();
  nav_container.addClass('navbar-collapse collapse');
  $('div.navbar').append(nav_container);
  $('#main-nav > ul').removeClass('sm sm-dox').addClass('nav navbar-nav navbar-right');
  $('#main-nav * li > ul').addClass('dropdown-menu');
  // replace '+' with caret
  var nav_sub = $('#main-nav * li > a.has-submenu');
  nav_sub.children('span').remove();
  nav_sub.append(' <span class="caret"></span>');

  /* enumerations */
  $('table.fieldtable').removeClass('fieldtable').addClass('table table-striped table-bordered').each(function(){
    $(this).prepend('<thead></thead>');
    $(this).find('tbody > tr:first').prependTo($(this).find('thead'));

    $(this).find('td > span.success').parent().addClass('success');
    $(this).find('td > span.warning').parent().addClass('warning');
    $(this).find('td > span.danger').parent().addClass('danger');
  });

  /* todo list */
  var todoelements = $('.contents > .textblock > dl.reflist > dt, .contents > .textblock > dl.reflist > dd');
  for (var i = 0; i < todoelements.length; i += 2) {
    $('.contents > .textblock').append(
      '<div class="panel panel-default active">'
      + "<div class=\"panel-heading todoname\">" + $(todoelements[i]).html() + "</div>"
      + "<div class=\"panel-body\">" + $(todoelements[i+1]).html() + "</div>"
      + '</div>');
  }
  $('.contents > .textblock > dl').remove();


  $(".memitem").removeClass('memitem');
  $(".memproto").removeClass('memproto');
  $(".memdoc").removeClass('memdoc');
  $("span.mlabel").removeClass('mlabel');
  $("table.memberdecls").removeClass('memberdecls');
  $("[class^=memitem]").removeClass('memitem');
  $("span.mlabels").removeClass('mlabels');
  $("table.mlabels").removeClass('mlabels');
  $("td.mlabels-right").removeClass('mlabels-right');
  $(".navpath").removeClass('navpath');
  $("li.navelem").removeClass('navelem');
  $("a.el").removeClass('el');
  $("div.ah").removeClass('ah');
  $("div.header").removeClass("header");
  $('h2.groupheader').removeClass('groupheader');

  /* separators */
  $("tr[class^='separator']").remove();

  /* custom */
  $("div.dyncontent").css("margin-bottom", "25px");

});
