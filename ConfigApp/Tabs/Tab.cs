using System.Windows.Controls;

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

        public override void Init(Grid grid)
        {
            base.Init(grid);

            InitContent();
        }
    }
}
