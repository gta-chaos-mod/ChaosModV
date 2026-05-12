using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs
{
    public abstract class Tab : ChaosGrid
    {
        protected abstract void InitContent();

        public virtual void OnTabSelected()
        {
        }

        public virtual void OnLoadValues()
        {
        }

        public virtual void OnSaveValues()
        {
        }

        public override void Init(Grid? grid)
        {
            base.Init(grid);
            InitContent();
        }
    }

    public abstract class CompositeTab : Tab
    {
        protected abstract IReadOnlyDictionary<string, Tab> ChildTabs { get; }

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(1f, GridUnitType.Star));
            SetRowHeight(new GridLength(1f, GridUnitType.Star));

            var tabControl = new TabView();
            foreach (var childTab in ChildTabs)
            {
                var tabItem = new TabViewItem { Header = childTab.Key };
                var grid = new Grid();
                childTab.Value.Init(grid);
                tabItem.Content = grid;
                tabControl.TabItems.Add(tabItem);
            }

            PushRowElement(tabControl);
            PopRow();
        }

        public override void OnLoadValues()
        {
            foreach (var childTab in ChildTabs.Values)
                childTab.OnLoadValues();
        }

        public override void OnSaveValues()
        {
            foreach (var childTab in ChildTabs.Values)
                childTab.OnSaveValues();
        }
    }
}
