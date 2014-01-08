/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

// Layer file, see http://www.dojotoolkit.org/reference-guide/build/index.html

// Any classes referenced in navigator.html need to be here.


define(
[

    // These are the dojo reqs from layer.js.

    "dojo/parser",


    // Any extra ones discovered by the build.

    "dijit/a11y",
    "dijit/a11yclick",
    "dijit/BackgroundIframe",
    "dijit/_base/focus",
    "dijit/_base/manager",
    "dijit/Calendar",
    "dijit/CalendarLite",
    "dijit/CheckedMenuItem",
    "dijit/_Contained",
    "dijit/_Container",
    "dijit/_CssStateMixin",
    "dijit/Destroyable",
    "dijit/Dialog",
    "dijit/_DialogMixin",
    "dijit/DialogUnderlay",
    "dijit/DropDownMenu",
    "dijit/focus",
    "dijit/_FocusMixin",
    "dijit/form/Button",
    "dijit/form/_ButtonMixin",
    "dijit/form/CheckBox",
    "dijit/form/_CheckBoxMixin",
    "dijit/form/DateTextBox",
    "dijit/form/_DateTimeTextBox",
    "dijit/form/DropDownButton",
    "dijit/form/Form",
    "dijit/form/_FormMixin",
    "dijit/form/_FormSelectWidget",
    "dijit/form/_FormValueMixin",
    "dijit/form/_FormValueWidget",
    "dijit/form/_FormWidget",
    "dijit/form/_FormWidgetMixin",
    "dijit/form/MappedTextBox",
    "dijit/form/MultiSelect",
    "dijit/form/nls/validate",
    "dijit/form/NumberTextBox",
    "dijit/form/RadioButton",
    "dijit/form/_RadioButtonMixin",
    "dijit/form/RangeBoundTextBox",
    "dijit/form/Select",
    "dijit/form/TextBox",
    "dijit/form/_TextBoxMixin",
    "dijit/form/TimeTextBox",
    "dijit/form/ToggleButton",
    "dijit/form/_ToggleButtonMixin",
    "dijit/form/ValidationTextBox",
    "dijit/_HasDropDown",
    "dijit/hccss",
    "dijit/_KeyNavContainer",
    "dijit/layout/BorderContainer",
    "dijit/layout/ContentPane",
    "dijit/layout/_ContentPaneResizeMixin",
    "dijit/layout/_LayoutWidget",
    "dijit/layout/ScrollingTabController",
    "dijit/layout/StackContainer",
    "dijit/layout/StackController",
    "dijit/layout/TabContainer",
    "dijit/layout/_TabContainerBase",
    "dijit/layout/TabController",
    "dijit/layout/utils",
    "dijit/main",
    "dijit/Menu",
    "dijit/_MenuBase",
    "dijit/MenuItem",
    "dijit/MenuSeparator",
    "dijit/nls/common",
    "dijit/nls/loading",
    "dijit/_OnDijitClickMixin",
    "dijit/place",
    "dijit/popup",
    "dijit/registry",
    "dijit/_TemplatedMixin",
    "dijit/_TimePicker",
    "dijit/Tooltip",
    "dijit/TooltipDialog",
    "dijit/typematic",
    "dijit/Viewport",
    "dijit/_Widget",
    "dijit/_WidgetBase",
    "dijit/_WidgetsInTemplateMixin",
    "dojo/cache",
    "dojo/cldr/nls/gregorian",
    "dojo/cldr/nls/number",
    "dojo/cldr/supplemental",
    "dojo/colors",
    "dojo/cookie",
    "dojo/data/ObjectStore",
    "dojo/data/util/sorter",
    "dojo/date",
    "dojo/date/locale",
    "dojo/dnd/autoscroll",
    "dojo/dnd/Avatar",
    "dojo/dnd/common",
    "dojo/dnd/Container",
    "dojo/dnd/Manager",
    "dojo/dnd/Moveable",
    "dojo/dnd/Mover",
    "dojo/dnd/Selector",
    "dojo/dnd/Source",
    "dojo/dnd/TimedMoveable",
    "dojo/fx/easing",
    "dojo/hccss",
    "dojo/html",
    "dojo/i18n",
    "dojo/number",
    "dojo/regexp",
    "dojo/request",
    "dojo/request/default",
    "dojo/Stateful",
    "dojo/store/JsonRest",
    "dojo/store/Memory",
    "dojo/store/util/QueryResults",
    "dojo/store/util/SimpleQueryEngine",
    "dojo/string",
    "dojo/text",
    "dojo/touch",
    "dojo/uacss",
    "dojo/window",
    "dojox/charting/action2d/Base",
    "dojox/charting/action2d/MoveSlice",
    "dojox/charting/action2d/PlotAction",
    "dojox/charting/action2d/Tooltip",
    "dojox/charting/axis2d/Base",
    "dojox/charting/axis2d/common",
    "dojox/charting/axis2d/Default",
    "dojox/charting/axis2d/Invisible",
    "dojox/charting/Chart",
    "dojox/charting/Element",
    "dojox/charting/plot2d/Base",
    "dojox/charting/plot2d/CartesianBase",
    "dojox/charting/plot2d/Columns",
    "dojox/charting/plot2d/common",
    "dojox/charting/plot2d/commonStacked",
    "dojox/charting/plot2d/Default",
    "dojox/charting/plot2d/Lines",
    "dojox/charting/plot2d/Pie",
    "dojox/charting/plot2d/_PlotEvents",
    "dojox/charting/plot2d/Stacked",
    "dojox/charting/plot2d/StackedAreas",
    "dojox/charting/plot2d/StackedColumns",
    "dojox/charting/scaler/common",
    "dojox/charting/scaler/linear",
    "dojox/charting/scaler/primitive",
    "dojox/charting/Series",
    "dojox/charting/SimpleTheme",
    "dojox/charting/Theme",
    "dojox/charting/themes/common",
    "dojox/charting/themes/Julie",
    "dojox/charting/widget/Legend",
    "dojox/color",
    "dojox/color/_base",
    "dojox/color/Palette",
    "dojox/encoding/base64",
    "dojox/gfx",
    "dojox/gfx/_base",
    "dojox/gfx/fx",
    "dojox/gfx/gradutils",
    "dojox/gfx/matrix",
    "dojox/gfx/renderer",
    "dojox/gfx/shape",
    "dojox/gfx/utils",
    "dojox/grid/_Builder",
    "dojox/grid/cells",
    "dojox/grid/cells/_base",
    "dojox/grid/DataGrid",
    "dojox/grid/DataSelection",
    "dojox/grid/_EditManager",
    "dojox/grid/enhanced/_Events",
    "dojox/grid/enhanced/_FocusManager",
    "dojox/grid/EnhancedGrid",
    "dojox/grid/enhanced/nls/EnhancedGrid",
    "dojox/grid/enhanced/_PluginManager",
    "dojox/grid/enhanced/plugins/_SelectionPreserver",
    "dojox/grid/_Events",
    "dojox/grid/_FocusManager",
    "dojox/grid/_Grid",
    "dojox/grid/_Layout",
    "dojox/grid/_RowManager",
    "dojox/grid/_RowSelector",
    "dojox/grid/_Scroller",
    "dojox/grid/Selection",
    "dojox/grid/_SelectionPreserver",
    "dojox/grid/util",
    "dojox/grid/_View",
    "dojox/grid/_ViewManager",
    "dojox/html/metrics",
    "dojox/lang/functional",
    "dojox/lang/functional/array",
    "dojox/lang/functional/fold",
    "dojox/lang/functional/lambda",
    "dojox/lang/functional/object",
    "dojox/lang/functional/reversed",
    "dojox/lang/functional/scan",
    "dojox/lang/utils",
    "dojox/main",
    "dojox/rpc/Rest",

    // These you see when loading Navigator in the browser.

    "dojo/selector/acme",

    "dojox/gfx/path",
    "dojox/gfx/svg"

],
function(
)
{

return {};

} );
